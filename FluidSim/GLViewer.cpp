#include "GLViewer.h"

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <sstream>
#include <vector>


#include "Shader.h"
#include "Log.h"
#include "Debug.h"
#include "SunLightSource.h"
#include "TriangleNetObject.h"
#include "VortonSim.h"
#include "VorticityDistribution.h"

#include <glm/geometric.hpp>

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

GLViewer::GLViewer(const char* titlePrefix, unsigned int width, unsigned int height) : m_width( width ), m_height( height ), m_title( titlePrefix ), m_TitlePrefix( titlePrefix ) {

	glfwSetErrorCallback(&errorFunction);

	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize glfw");
	}

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

	std::string cullMsg = std::string("Cull faces is ") + (glIsEnabled(GL_CULL_FACE) ? "enabled" : "disabled");
	DEBUG(cullMsg);

	/*
	 * Setup scene
	*/
	SunLightSource light{m_Scene, 0.8f, glm::vec3(0.0f, 0.0f, -1.0f) };
	ShaderLightSourceVariable lightSrcVar{ "sunLight", LightSourceType::SUNLIGHT };

	Material sphereMaterial{ glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0.2f), 10 };
	Material &sphereMatRef = m_Scene.addMaterial(sphereMaterial);

	/*
	* setup scene: setup program
	*/
	std::vector<ShaderLightSourceVariable> lightSrcVars;
	lightSrcVars.push_back(lightSrcVar);

	Program &prog = m_Scene.addProgram(lightSrcVars);

	Shader vertexShader{ GL_VERTEX_SHADER };
	vertexShader.loadSourceFromFile("shaders\\basic3D.vert");

	Shader fragmentShader{ GL_FRAGMENT_SHADER };
	fragmentShader.loadSourceFromFile("shaders\\basic3D_phong.frag");

	prog.attachShader(&vertexShader);
	prog.attachShader(&fragmentShader);

	vertexShader.compile();
	fragmentShader.compile();
	prog.link();
	prog.detachAllShaders();

	m_Scene.addObject(light);

	//setup geometry
	m_Scene.aspectRatio(static_cast<float>(width) / height);
	Geometry &geomSphere = m_Scene.addGeometryFromFile("objects\\sphere.obj");
	geomSphere.setupAttribArrays(prog);

	//create vorton prototype
	m_VortonPrototypePtr = std::make_unique<TriangleNetObject>(&sphereMatRef, &geomSphere, &prog);
	m_VortonPrototypePtr->scale(glm::dvec3(0.03));

	setupVortonSim(false);
	
	m_Scene.camera().translate(glm::dvec3(0, 6, 12));
	m_Scene.camera().rotateLocalX(glm::radians(-20.0));
	m_Scene.startStepping();
}

GLViewer::~GLViewer()
{
	glfwDestroyWindow(m_WindowPtr);
	glfwTerminate();
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
}


void GLViewer::cycle()
{
	incrementFrameCount();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_Scene.render();

	glfwSwapBuffers(m_WindowPtr);
	glfwPollEvents();
}

bool GLViewer::shouldClose() const
{
	return (glfwWindowShouldClose(m_WindowPtr) == 1);
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
}

void GLViewer::mouseMotionFunction(GLFWwindow *windowPtr, double x, double y)
{
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

void GLViewer::setupVortonSim(bool createPaused)
{
	m_VortonSimPtr = new VortonSim(0.05, 1.0, JetRingVorticityDistribution(glm::dvec3(0), 1.0, 1.0, glm::dvec3(1.0, 0.0, 0.0)), 20.0, *m_VortonPrototypePtr);
	m_VortonSimPtr->simulating(!createPaused);
	m_Scene.addObjectPtr(m_VortonSimPtr);
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

VortonSim & GLViewer::vortonSim()
{
	if (m_VortonSimPtr == nullptr) {
		throw std::runtime_error("GLViewer::vortonSim() m_VortonSimPtr is nullptr");
	}
	return *m_VortonSimPtr;
}

void GLViewer::resetSim(bool createPaused)
{
	if (m_VortonSimPtr != nullptr) {
		m_Scene.removeObjectPtr(m_VortonSimPtr);
		m_VortonSimPtr->inUpdateMutex().lock();
		m_VortonSimPtr->inUpdateMutex().unlock();
		delete m_VortonSimPtr;
	}
	setupVortonSim(createPaused);
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

