#include "GLViewer.h"

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <sstream>
#include <vector>


#include "../graphics/Shader.h"
#include "../util/Log.h"
#include "../util/Debug.h"
#include "../graphics/SunLightSource.h"
#include "../graphics/TriangleNetObject.h"
#include "../simulation/VorticityDistribution.h"

#include "../simulation/EmitParticlesOperation.h"
#include "../simulation/AdvectParticlesOperation.h"
#include "../simulation/KillParticlesByAgeOperation.h"
#include "../simulation/SolveBoundaryConditionsOperation.h"
#include "../simulation/SolveBoundaryConditionsVortonsOperation.h"
#include "../simulation/UpdateFluidOperation.h"
#include "../simulation/ApplyVelocityGridOperation.h"
#include "../simulation/WindOperation.h"

#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>

#define DO_GUI 1

GLViewer* GLViewer::m_instance = nullptr;
const float GLViewer::m_MOUSE_TRANSLATION_TO_CAMERA_ROTATION = -0.001f;

void GLViewer::initInstance(const char* titlePrefix, unsigned int width, unsigned int height) {
	if (m_instance != nullptr) {
		throw std::runtime_error("Already initialized");
	}

	m_instance = new GLViewer(titlePrefix, width, height);
}

GLViewer* GLViewer::instance() {
	return m_instance;
}

void GLViewer::deleteInstance() {
	if (m_instance != nullptr) {
		delete m_instance;
		m_instance = nullptr;
	}
	else {
		throw std::runtime_error("No instance available to delete");
	}
}

GLViewer::GLViewer(const char* titlePrefix, unsigned int width, unsigned int height)
    : m_width( width ), m_height( height ), m_title( titlePrefix ), m_TitlePrefix( titlePrefix ),
    m_MouseRotationReady{ false }
{

#if DO_GUI
	glfwSetErrorCallback(&errorFunction);
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize glfw");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	m_WindowPtr = glfwCreateWindow(width, height, titlePrefix, nullptr, nullptr);
	if (m_WindowPtr == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Failed to create window");
	}

	glfwMakeContextCurrent(m_WindowPtr);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(m_WindowPtr, &framebufferResizeFunction);
	glfwSetMouseButtonCallback(m_WindowPtr, &mouseButtonFunction);
	glfwSetCursorPosCallback(m_WindowPtr, &mouseMotionFunction);
	glfwSetKeyCallback(m_WindowPtr, &keyboardFunction);

	glewExperimental = GL_TRUE;
	GLenum glewInitResult = glewInit();
	if (glewInitResult != GLEW_OK) {
		std::cout << "glewInit() failed: " << glewGetErrorString(glewInitResult) << std::endl;
		throw std::runtime_error("glewInit() failed");
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	std::cout << "Initialized, OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	GLenum err = glGetError();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
    glLineWidth(1.0f);

	std::string cullMsg = std::string("Cull faces is ") + (glIsEnabled(GL_CULL_FACE) ? "enabled" : "disabled");
	DEBUG(cullMsg);

	/*
	 * Setup scene
	*/
	SunLightSource light{ m_Scene, 0.8f, glm::vec3(0.0f, 0.0f, -1.0f) };

	m_Scene.addObject(light);
#endif
	setupSim(false);
#if DO_GUI
	m_Scene.camera().translate(glm::dvec3(0, 6, 12));
	m_Scene.camera().rotateLocalX(glm::radians(-20.0));
#endif
	m_Scene.startStepping();
#if DO_GUI
	m_MainFramebufferPtr = std::make_unique<Framebuffer>();
	attachTexturesToFBO();
	m_MainFramebufferPtr->addDrawBuffer(GL_COLOR_ATTACHMENT0);
	m_MainFramebufferPtr->addDrawBuffer(GL_COLOR_ATTACHMENT1);

	if (!m_MainFramebufferPtr->isComplete()) {
		throw std::runtime_error("GLViewer::GLViewer: Main Framebuffer isn't complete!");
	}
#endif
}

void GLViewer::attachTexturesToFBO()
{
	m_MainColorTexturePtr = std::make_unique<Texture2DFixedSize>(GL_RGB8, m_width, m_height);
	m_MainColorTexturePtr->minFilter(GL_NEAREST);
	m_ObjectIndexTexturePtr = std::make_unique<Texture2DFixedSize>(GL_R32F, m_width, m_height);
	m_MainDepthTexturePtr = std::make_unique<Texture2DFixedSize>(GL_DEPTH_COMPONENT24, m_width, m_height);

	m_MainFramebufferPtr->attachTexture(GL_COLOR_ATTACHMENT0, *m_MainColorTexturePtr);
	m_MainFramebufferPtr->attachTexture(GL_COLOR_ATTACHMENT1, *m_ObjectIndexTexturePtr);
	m_MainFramebufferPtr->attachTexture(GL_DEPTH_ATTACHMENT, *m_MainDepthTexturePtr);
}

void GLViewer::handleRightclick(glm::dvec2 &cursorPosition)
{
	GLuint objectIndex;
	float objectIndexF;
	cursorPosition.y = m_height - cursorPosition.y;	//inverted version needed

	const float MAX_UINT = std::pow(2.0, 32) - 1;
	m_MainFramebufferPtr->readPixels(GL_COLOR_ATTACHMENT1, glm::ivec2(cursorPosition), glm::ivec2(1), GL_RED, GL_FLOAT, &objectIndexF);
	
	objectIndex = static_cast<unsigned int>(objectIndexF * MAX_UINT);
	if (objectIndex == 0) {
		INFO("No Object at this location");
		return;
	}
	objectIndex--;
	Object::objectWithId(objectIndex).printInfo();
	try {
		Arrow& arrow = dynamic_cast<Arrow&>(Object::objectWithId(objectIndex));
		arrow.toggleHighlight();
	}
	catch (const std::bad_cast& error) {
		INFO("NOT AN ARROW--> No Highlight");
	}
}

GLViewer::~GLViewer()
{
#if DO_GUI
	glfwDestroyWindow(m_WindowPtr);
	glfwTerminate();
#endif
}

void GLViewer::errorFunction(int error, const char * description)
{
	std::cerr << "GLFW error: " << description << std::endl;
}

void GLViewer::framebufferResizeFunction(GLFWwindow * windowPtr, int width, int height)
{
	glViewport(0, 0, width, height);
	GLViewer* viewer = instance();
	if (viewer == nullptr) {
		throw std::runtime_error("ResizeFunction: Instance of GLViewer doesn't exist");
	}
	viewer->m_Scene.aspectRatio(static_cast<float>(width) / height);
	viewer->width(width);
	viewer->height(height);
	viewer->attachTexturesToFBO();
}


void GLViewer::cycle()
{
#if DO_GUI
	incrementFrameCount();
	m_MainFramebufferPtr->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_MainFramebufferPtr->render(m_Scene);
	m_MainFramebufferPtr->blitColorToDefaultFBO(
		GL_COLOR_ATTACHMENT0, glm::ivec2(0), glm::ivec2(m_width, m_height), glm::ivec2(0), glm::ivec2(m_width, m_height)
	);
#endif
	while (!m_PerformAfterRender.empty()) {
		(m_PerformAfterRender.front())();
		m_PerformAfterRender.pop();
	}
#if DO_GUI
	glfwSwapBuffers(m_WindowPtr);
	glfwPollEvents();
#endif
}

bool GLViewer::shouldClose() const
{
#if DO_GUI
	return (glfwWindowShouldClose(m_WindowPtr) == 1);
#endif
    return false;
}

void GLViewer::width(int width) {
	assert(width >= 0);
	m_width = width;
}

void GLViewer::height(int height) {
	assert(height >= 0);
	m_height = height;
}

/*void GLViewer::timerFPS(int value) {
	GLViewer* viewer = GLViewer::instance();
	if (viewer == nullptr) {
		throw std::runtime_error("Timer_FPS: instance of GLViewer doesn't exist");
	}
	if (value != 0) {	//if not first time called
		unsigned int fps = viewer->frameCount()*(1000 / m_UPDATE_INTERVAL_MS);

		std::stringstream title;
		title << viewer->titlePrefix() << ": " << fps << " FPS @ " << viewer->width() << " x " << viewer->height();
		viewer->title(title.str());
	}
	viewer->frameCount(0);
	glutTimerFunc(m_UPDATE_INTERVAL_MS, timerFPS, 1);	//we do not call for the first time -> pass value 1
}*/

void GLViewer::keyboardFunction(GLFWwindow *windowPtr, int key, int scancode, int action, int mods)
{
	GLViewer* viewer = GLViewer::instance();
	if (viewer == nullptr) {
		throw std::runtime_error("KeyboardFunction: instance of GLViewer doesn't exist");
	}
	if (action == GLFW_PRESS) {
		viewer->keyboardFunctionDown(key);
	}
	else if (action == GLFW_RELEASE) {
		viewer->keyboardFunctionUp(key);
	}
}

void GLViewer::keyboardFunctionDown(int key)
{
	if ((m_KeysPressedState.count(key) == 0) || !(m_KeysPressedState[key])) {
		switch (key) {
		case GLFW_KEY_W:
			m_Scene.camera().startMoveForward();
			break;
		case GLFW_KEY_A:
			m_Scene.camera().startMoveLeft();
			break;
		case GLFW_KEY_D:
			m_Scene.camera().startMoveRight();
			break;
		case GLFW_KEY_S:
			m_Scene.camera().startMoveBack();
			break;
		}
	}
	m_KeysPressedState[key] = true;
}

void GLViewer::keyboardFunctionUp(int key)
{
	switch (key) {
	case GLFW_KEY_W:
		m_Scene.camera().stopMoveForward();
		break;
	case GLFW_KEY_A:
		m_Scene.camera().stopMoveLeft();
		break;
	case GLFW_KEY_D:
		m_Scene.camera().stopMoveRight();
		break;
	case GLFW_KEY_S:
		m_Scene.camera().stopMoveBack();
		break;
	}
	m_KeysPressedState[key] = false;
}

void GLViewer::mouseButtonFunction(GLFWwindow *windowPtr, int button, int action, int mods)
{
	GLViewer* viewer = GLViewer::instance();
	if (viewer == nullptr) {
		throw std::runtime_error("MouseFunction: instance of GLViewer doesn't exist");
	}

	viewer->m_MouseRotationReady = (action == GLFW_PRESS) ? true : false;
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
		glm::dvec2 cursorPos;
		glfwGetCursorPos(windowPtr, &cursorPos.x, &cursorPos.y);

		std::function<void()> rightclickCall = std::bind(&GLViewer::handleRightclick, viewer, cursorPos);
		viewer->m_PerformAfterRender.push(rightclickCall);
	}
}

void GLViewer::mouseMotionFunction(GLFWwindow *windowPtr, double x, double y)
{
    return;
	glm::vec2 currentCoordinates = glm::vec2(x, y);

	GLViewer* viewer = GLViewer::instance();
	if (viewer == nullptr) {
		throw std::runtime_error("MouseMotionFunction: instance of GLViewer doesn't exist");
	}
	if (viewer->m_MouseRotationReady) {
		glm::vec2 delta = currentCoordinates - viewer->m_LastMouseCoordinates;

		double rotationXDeg = m_MOUSE_TRANSLATION_TO_CAMERA_ROTATION * delta.y;
		double rotationYDeg = m_MOUSE_TRANSLATION_TO_CAMERA_ROTATION * delta.x;

		Camera &camera = viewer->m_Scene.camera();
		camera.rotateLocalX(rotationXDeg);
		camera.rotateLocalY(rotationYDeg);
	}
	
	viewer->m_LastMouseCoordinates = currentCoordinates;
}

void GLViewer::setupSim(bool createPaused)
{
    m_RigidBodySimPtr = createRigidBodySim();
    m_RigidBodySimPtr->simulating(!createPaused);

	//std::vector<const VorticityDistribution*> initialVorticityPtrs;
    //initialVorticityPtrs.push_back(new NoiseVorticityDistribution(glm::dvec3(0), glm::dvec3(4.0, 0.5, 0.5)));
	//initialVorticityPtrs.push_back(new JetRingVorticityDistribution(glm::dvec3(0), 1, 1, glm::dvec3(1.0, 0.0, 0.0)));
	//initialVorticityPtrs.push_back(new VortexTubeVorticityDistribution(glm::dvec3(0), 0.5, 0.0, 2.0, 2, -1));
	//m_VortonSimPtr = new VortonSim(*m_RigidBodySimPtr, 0.05, 1.0, initialVorticityPtrs, 0.125 * FLT_EPSILON);
	//m_VortonSimPtr->simulating(!createPaused);
	//m_Scene.addObjectPtr(m_VortonSimPtr);

    constexpr size_t vortonsPerDimension = 16u;
    constexpr size_t vortonsTotal = vortonsPerDimension * vortonsPerDimension * vortonsPerDimension;
    constexpr size_t tracersPerDimension = 2 * vortonsPerDimension;
    constexpr size_t tracersTotal = tracersPerDimension * tracersPerDimension * tracersPerDimension;
    constexpr float extentPerDimension = 1.0f;
    constexpr float density = 1.0;
    constexpr double vortonRadius = (extentPerDimension / vortonsPerDimension) * 0.5;
    constexpr double vortonMass = (extentPerDimension * extentPerDimension * extentPerDimension) * density / (vortonsTotal);

    constexpr double tracerRadius = (extentPerDimension / tracersPerDimension) * 0.5;
    constexpr double tracerMass = 1e-3;

	constexpr double particleLifetime = 5.0;

    m_VortonParticleSystemPtr = createParticleSystem();
    m_TracerParticleSystemPtr = createParticleSystem();

    std::vector<std::reference_wrapper<ParticleSystem>> fluidOperationRespectedParticleSystems{ *m_VortonParticleSystemPtr, *m_TracerParticleSystemPtr };
    std::vector<std::reference_wrapper<RigidBodySim>> fluidOperationRespectedRigidBodySims{*m_RigidBodySimPtr};
    auto updateFluidOperation = std::make_unique<UpdateFluidOperation>(*m_VortonParticleSystemPtr, 1.0, fluidOperationRespectedParticleSystems, fluidOperationRespectedRigidBodySims);
    UpdateFluidOperation* updateFluidOperationRawPtr = updateFluidOperation.get();
    m_VortonParticleSystemPtr->addParticleOperation(std::move(updateFluidOperation));

    auto solveVortonBoundaryConditionsOperation = std::make_unique<SolveBoundaryConditionsVortonsOperation>(*m_VortonParticleSystemPtr, *m_RigidBodySimPtr, std::bind(std::mem_fn(&UpdateFluidOperation::velocityGridPtr), updateFluidOperationRawPtr));
    m_VortonParticleSystemPtr->addParticleOperation(std::move(solveVortonBoundaryConditionsOperation));

	auto getVelocityGridLambda = [updateFluidOperationRawPtr](){
		return updateFluidOperationRawPtr->velocityGridPtr().get();
	};
	auto vortonWindOperation = std::make_unique<WindOperation>(*m_VortonParticleSystemPtr, glm::dvec3{1.0, 0.0, 0.0}, 1.0);
	m_VortonParticleSystemPtr->addParticleOperation(std::move(vortonWindOperation));

	auto vortonApplyVelocityGridOperation = std::make_unique<ApplyVelocityGridOperation>(*m_VortonParticleSystemPtr, getVelocityGridLambda);
	m_VortonParticleSystemPtr->addParticleOperation(std::move(vortonApplyVelocityGridOperation));

    auto advectVortonsOperation = std::make_unique<AdvectParticlesOperation>(*m_VortonParticleSystemPtr);
    m_VortonParticleSystemPtr->addParticleOperation(std::move(advectVortonsOperation));

    auto emitVortonOperation =
        std::make_unique<EmitParticlesOperation>(
            *m_VortonParticleSystemPtr,
            vortonsTotal / particleLifetime,
            glm::dvec3{ 1.0, 0.0, 0.0 }, glm::dvec3{ 0 },
            glm::dvec3{ -3, 0, 0 }, glm::dvec3{ 0, 1, 1 },
            vortonRadius, vortonMass,
            [](const double birthTimeSeconds, const double initialRadius, const double initialMass)
    {
        return std::make_unique<Vorton>(birthTimeSeconds, initialRadius, initialMass);
    });
    m_VortonParticleSystemPtr->addParticleOperation(std::move(emitVortonOperation));

    auto killVortonParticleOperation = std::make_unique<KillParticlesByAgeOperation>(*m_VortonParticleSystemPtr, particleLifetime);
    m_VortonParticleSystemPtr->addParticleOperation(std::move(killVortonParticleOperation));

    auto solveTracerBoundaryConditionsOperation = std::make_unique<SolveBoundaryConditionsOperation>(*m_TracerParticleSystemPtr, *m_RigidBodySimPtr);
    m_TracerParticleSystemPtr->addParticleOperation(std::move(solveTracerBoundaryConditionsOperation));

	auto tracerWindOperation = std::make_unique<WindOperation>(*m_TracerParticleSystemPtr, glm::dvec3{1.0, 0.0, 0.0}, 1.0);
	m_TracerParticleSystemPtr->addParticleOperation(std::move(tracerWindOperation));

	auto tracerApplyVelocityGridOperation = std::make_unique<ApplyVelocityGridOperation>(*m_TracerParticleSystemPtr, getVelocityGridLambda);
	m_TracerParticleSystemPtr->addParticleOperation(std::move(tracerApplyVelocityGridOperation));

    auto advectTracersOperation = std::make_unique<AdvectParticlesOperation>(*m_TracerParticleSystemPtr);
    m_TracerParticleSystemPtr->addParticleOperation(std::move(advectTracersOperation));

    auto emitTracerOperation =
        std::make_unique<EmitParticlesOperation>(
            *m_TracerParticleSystemPtr,
            tracersTotal / particleLifetime,
            glm::dvec3{ 1, 0.0, 0.0 }, glm::dvec3{ 0 },
            glm::dvec3{ -3.0, 0.0, 0.0 }, glm::dvec3{ 0.0, 1.0, 1.0 },
            tracerRadius, tracerMass,
            [](const double birthTimeSeconds, const double initialRadius, const double initialMass)
    {
        return std::make_unique<Particle>(birthTimeSeconds, initialRadius, initialMass);
    });
    m_TracerParticleSystemPtr->addParticleOperation(std::move(emitTracerOperation));

    auto killParticleOperation = std::make_unique<KillParticlesByAgeOperation>(*m_TracerParticleSystemPtr, particleLifetime);
    m_TracerParticleSystemPtr->addParticleOperation(std::move(killParticleOperation));

#if DO_GUI
    createVortonSimRenderer(updateFluidOperationRawPtr);
#endif
}

ParticleSystem* GLViewer::createParticleSystem()
{
    return dynamic_cast<ParticleSystem*>(m_Scene.addObjectPtr(std::make_unique<ParticleSystem>()));
}

void GLViewer::createVortonSimRenderer(UpdateFluidOperation* updateFluidOperationPtr)
{
	auto addedObjectPtr = m_Scene.addObjectPtr(std::make_unique<VortonSimRenderer>(
		*m_VortonParticleSystemPtr, *m_TracerParticleSystemPtr,
		*updateFluidOperationPtr, m_Scene));
	m_VortonSimRendererPtr = dynamic_cast<VortonSimRenderer*>(addedObjectPtr);
}

RigidBodySim* GLViewer::createRigidBodySim()
{
    return dynamic_cast<RigidBodySim*>(m_Scene.addObjectPtr(std::make_unique<RigidBodySim>()));
}

unsigned int GLViewer::frameCount() const {
	return m_FrameCount;
}

void GLViewer::frameCount(const unsigned int count) {
	m_FrameCount = count;
}

void GLViewer::incrementFrameCount() {
	m_FrameCount++;
}

Scene & GLViewer::scene()
{
	return m_Scene;
}

RigidBodySim & GLViewer::rigidBodySim()
{
    return *m_RigidBodySimPtr;
}

VortonSimRenderer & GLViewer::vortonSimRenderer()
{
	return *m_VortonSimRendererPtr;
}

void GLViewer::resetSim(bool createPaused)
{
    /*
    if (m_RigidBodySimPtr != nullptr) {
        m_Scene.removeObjectPtr(m_RigidBodySimPtr);
        m_RigidBodySimPtr->inUpdateMutex().lock();
        m_RigidBodySimPtr->inUpdateMutex().unlock();
        delete m_RigidBodySimPtr;
    }
	if (m_TracerParticleSystemPtr != nullptr) {
		m_Scene.removeObjectPtr(m_TracerParticleSystemPtr);
		
		delete m_TracerParticleSystemPtr;
	}
	setupSim(createPaused);
	m_VortonSimRendererPtr = new VortonSimRenderer{ *m_VortonSimPtr, m_Scene };
	m_Scene.addObjectPtr(m_VortonSimRendererPtr);
    */
}

void GLViewer::title(const std::string& title) {
	glfwSetWindowTitle(m_WindowPtr, title.c_str());
	m_title = title;
}

int GLViewer::width() const {
	return m_width;
}

int GLViewer::height() const {
	return m_height;
}

std::string GLViewer::titlePrefix() const {
	return m_TitlePrefix;
}

