#include "GLViewer.h"

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
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

void GLViewer::initInstance(const char* titlePrefix, unsigned int width, unsigned int height, int argc, char* argv[]) {
	if (m_instance != nullptr) {
		throw std::runtime_error("Already initialized");
	}
	glutInit(&argc, argv);

	glutInitContextVersion(4, 5);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	m_instance = new GLViewer(titlePrefix, width, height, argc, argv);
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

GLViewer::GLViewer(const char* titlePrefix, unsigned int width, unsigned int height, int argc, char* argv[]) : m_width( width ), m_height( height ), m_title( titlePrefix ), m_TitlePrefix( titlePrefix ) {
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	glutInitWindowSize(width, height);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	m_WindowHandle = glutCreateWindow(titlePrefix);

	if (m_WindowHandle < 1) {
		throw std::runtime_error("Failed to create window");
	}

	glutReshapeFunc(&resizeFunction);
	glutDisplayFunc(&renderFunction);
	glutIdleFunc(&idleFunction);
	glutTimerFunc(0, &timerFPS, 0);	//call Timer_FPS immediately if everything is set up
	glutCloseFunc(&cleanup);

	glutMouseFunc(&mouseFunction);
	glutMotionFunc(&mouseMotionFunction);
	glutKeyboardFunc(&keyboardFunction);
	glutKeyboardUpFunc(&keyboardFunctionUp);

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

	Material sphereMaterial{ glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(0.2f), 10 };
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

	//create and add object to scene
	TriangleNetObject vortonPrototype{ m_Scene, &sphereMatRef, &geomSphere, &prog };
	vortonPrototype.scale(glm::vec3(0.01f));

	m_VortonSimPtr = new VortonSim(m_Scene, 0.05f, 1.0f, JetRingVorticityDistribution(glm::vec3(0), 1.0f, 1.0f, glm::vec3(1.0, 0.0, 0.0)), 20.0f, vortonPrototype);
	m_Scene.addObjectPtr(m_VortonSimPtr);
	
	m_Scene.camera().translate(glm::vec3(0, 6, 12));
	m_Scene.camera().rotateLocalX(glm::radians(-20.0f));
	m_Scene.startStepping();
}

GLViewer::~GLViewer()
{
	
}

void GLViewer::resizeFunction(int width, int height) {
	glViewport(0, 0, width, height);
	GLViewer* viewer = instance();
	if (viewer == nullptr) {
		throw std::runtime_error("ResizeFunction: Instance of GLViewer doesn't exist");
	}
	viewer->m_Scene.aspectRatio(static_cast<float>(width) / height);
	viewer->width(width);
	viewer->height(height);
}

void GLViewer::renderFunction(void) {
	GLViewer* viewer = GLViewer::instance();
	if (viewer == nullptr) {
		throw std::runtime_error("RenderFunction: instance of GLViewer doesn't exist");
	}
	viewer->incrementFrameCount();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewer->m_Scene.render();

	glutSwapBuffers();
	glutPostRedisplay();
}

void GLViewer::MainLoop(void) {
	if (GLViewer::instance() == nullptr) {
		throw std::runtime_error("GLViewer::MainLoop() called but GLViewer isn't initialized");
	}
	glutMainLoop();
}

void GLViewer::width(int width) {
	assert(width >= 0);
	m_width = width;
}

void GLViewer::height(int height) {
	assert(height >= 0);
	m_height = height;
}

void GLViewer::idleFunction(void) {
	glutPostRedisplay();
}

void GLViewer::timerFPS(int value) {
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
}

void GLViewer::keyboardFunction(unsigned char key, int x, int y)
{
	GLViewer* viewer = GLViewer::instance();
	if (viewer == nullptr) {
		throw std::runtime_error("KeyboardFunction: instance of GLViewer doesn't exist");
	}
	if ((viewer->m_KeysPressedState.count(key) == 0) || !(viewer->m_KeysPressedState[key])) {
		switch (key) {
		case 'w':
			viewer->m_Scene.camera().startMoveForward();
			break;
		case 'a':
			viewer->m_Scene.camera().startMoveLeft();
			break;
		case 'd':
			viewer->m_Scene.camera().startMoveRight();
			break;
		case 's':
			viewer->m_Scene.camera().startMoveBack();
			break;
		}
	}
	viewer->m_KeysPressedState[key] = true;
}

void GLViewer::keyboardFunctionUp(unsigned char key, int x, int y)
{
	GLViewer* viewer = GLViewer::instance();
	if (viewer == nullptr) {
		throw std::runtime_error("KeyboardFunction: instance of GLViewer doesn't exist");
	}
	switch (key) {
	case 'w':
		viewer->m_Scene.camera().stopMoveForward();
		break;
	case 'a':
		viewer->m_Scene.camera().stopMoveLeft();
		break;
	case 'd':
		viewer->m_Scene.camera().stopMoveRight();
		break;
	case 's':
		viewer->m_Scene.camera().stopMoveBack();
		break;
	}
	viewer->m_KeysPressedState[key] = false;
}

void GLViewer::mouseFunction(int button, int state, int x, int y)
{
	GLViewer* viewer = GLViewer::instance();
	if (viewer == nullptr) {
		throw std::runtime_error("MouseFunction: instance of GLViewer doesn't exist");
	}

	viewer->m_LastMouseCoordinates = glm::vec2(x, y);
	viewer->m_MouseRotationReady = (state == GLUT_DOWN) ? true : false;
}

void GLViewer::mouseMotionFunction(int x, int y)
{
	GLViewer* viewer = GLViewer::instance();
	if (viewer == nullptr) {
		throw std::runtime_error("MouseMotionFunction: instance of GLViewer doesn't exist");
	}
	if (!viewer->m_MouseRotationReady) {
		return;
	}

	glm::vec2 currentCoordinates = glm::vec2(x, y);
	glm::vec2 delta = currentCoordinates - viewer->m_LastMouseCoordinates;

	float rotationXDeg = m_MOUSE_TRANSLATION_TO_CAMERA_ROTATION * delta.y;
	float rotationYDeg = m_MOUSE_TRANSLATION_TO_CAMERA_ROTATION * delta.x;
	
	Camera &camera = viewer->m_Scene.camera();
	camera.rotateLocalX(rotationXDeg);
	camera.rotateLocalY(rotationYDeg);
	
	viewer->m_LastMouseCoordinates = currentCoordinates;
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

void GLViewer::title(const std::string& title) {
	glutSetWindowTitle(title.c_str());
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

void GLViewer::cleanup()
{
	try {
		GLViewer::deleteInstance();
	}
	catch (const std::exception& ex) {
		ERROR_MSG("In cleanup(): " << ex.what());
	}
}
