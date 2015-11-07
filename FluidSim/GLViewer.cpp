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

GLViewer* GLViewer::m_instance = nullptr;

void GLViewer::initInstance(const char* titlePrefix, unsigned int width, unsigned int height, int argc, char* argv[]) {
	if (m_instance != nullptr) {
		throw std::runtime_error("Already initialized");
	}
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	m_instance = new GLViewer(titlePrefix, width, height, argc, argv);
}

GLViewer* GLViewer::getInstance() {
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

GLViewer::GLViewer(const char* titlePrefix, unsigned int width, unsigned int height, int argc, char* argv[]) : m_width{ width }, m_height{ height }, m_title{ titlePrefix }, m_TitlePrefix{ titlePrefix } {
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

	glutReshapeFunc(&ResizeFunction);
	glutDisplayFunc(&RenderFunction);
	glutIdleFunc(&IdleFunction);
	glutTimerFunc(0, &Timer_FPS, 0);	//call Timer_FPS immediately if everything is set up
	glutCloseFunc(&cleanup);

	glewExperimental = GL_TRUE;
	GLenum glewInitResult = glewInit();
	if (glewInitResult != GLEW_OK) {
		std::cout << "glewInit() failed: " << glewGetErrorString(glewInitResult) << std::endl;
		throw std::runtime_error("glewInit() failed");
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	std::cout << "Initialized, OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	GLenum err = glGetError();

	glDisable(GL_CULL_FACE);

	std::string cullMsg = std::string("Cull faces is ") + (glIsEnabled(GL_CULL_FACE) ? "enabled" : "disabled");
	DEBUG(cullMsg);

	setupTriangle();
}

GLViewer::~GLViewer()
{
	try {
		cleanupTriangle();
	}
	catch (const std::exception& ex) {
		ERROR_MSG("In destructor: " << ex.what());
	}
}

void GLViewer::ResizeFunction(int width, int height) {
	glViewport(0, 0, width, height);
	GLViewer* viewer = getInstance();
	if (viewer == nullptr) {
		throw std::runtime_error("ResizeFunction: Instance of GLViewer doesn't exist");
	}
	viewer->setWidth(width);
	viewer->setHeight(height);
}

void GLViewer::RenderFunction(void) {
	GLViewer* viewer = GLViewer::getInstance();
	if (viewer == nullptr) {
		throw std::runtime_error("RenderFunction: instance of GLViewer doesn't exist");
	}
	viewer->incrementFrameCount();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewer->drawTriangle();

	glutSwapBuffers();
	glutPostRedisplay();
}

void GLViewer::MainLoop(void) {
	if (GLViewer::getInstance() == nullptr) {
		throw std::runtime_error("GLViewer::MainLoop() called but GLViewer isn't initialized");
	}
	glutMainLoop();
}

void GLViewer::setWidth(int width) {
	assert(width >= 0);
	m_width = width;
}

void GLViewer::setHeight(int height) {
	assert(height >= 0);
	m_height = height;
}

void GLViewer::IdleFunction(void) {
	glutPostRedisplay();
}

void GLViewer::Timer_FPS(int value) {
	GLViewer* viewer = GLViewer::getInstance();
	if (viewer == nullptr) {
		throw std::runtime_error("Timer_FPS: instance of GLViewer doesn't exist");
	}
	if (value != 0) {	//if not first time called
		unsigned int fps = viewer->getFrameCount()*(1000 / m_UPDATE_INTERVAL_MS);

		std::stringstream title;
		title << viewer->getTitlePrefix() << ": " << fps << " FPS @ " << viewer->getWidth() << " x " << viewer->getHeight();
		viewer->setTitle(title.str());
	}
	viewer->setFrameCount(0);
	glutTimerFunc(m_UPDATE_INTERVAL_MS, Timer_FPS, 1);	//we do not call for the first time -> pass value 1
}

unsigned int GLViewer::getFrameCount() const {
	return m_FrameCount;
}

void GLViewer::setFrameCount(const unsigned int count) {
	m_FrameCount = count;
}

void GLViewer::incrementFrameCount() {
	m_FrameCount++;
}

void GLViewer::setTitle(const std::string& title) {
	glutSetWindowTitle(title.c_str());
	m_title = title;
}

int GLViewer::getWidth() const {
	return m_width;
}

int GLViewer::getHeight() const {
	return m_height;
}

std::string GLViewer::getTitlePrefix() const {
	return m_TitlePrefix;
}

void GLViewer::setupTriangle()
{
	std::vector<GLfloat> trianglePos{
		-0.8f, -0.8f, 0.0f,
		0.8f, -0.8f, 0.0f,
		0.0f, 0.8f, 0.0f };

	std::vector<GLfloat> triangleColor{
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f
		};

	GLenum errorCheckVal = glGetError();

	m_TriangleVao = new VertexArrO(true);

	m_TrianglePosVbo = new VertexBufO(true);
	m_TrianglePosVbo->pushData(trianglePos, GL_STATIC_DRAW, false);

	m_TriangleColorVbo = new VertexBufO(true);
	m_TriangleColorVbo->pushData(triangleColor, GL_STATIC_DRAW, false);

	m_TriangleVao->addVertexAttribArray(*m_TrianglePosVbo, false, true, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	m_TriangleVao->addVertexAttribArray(*m_TriangleColorVbo, false, true, 1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	m_TriangleVao->enableVertexAttribArray(false, 0);
	m_TriangleVao->enableVertexAttribArray(false, 1);
	
	Shader vertSh(GL_VERTEX_SHADER);
	vertSh.setSourcePath("C:\\Users\\erik\\Documents\\Visual Studio 2013\\Projects\\FluidSim\\FluidSim\\shaders\\basic.vert");

	Shader fragSh(GL_FRAGMENT_SHADER);
	fragSh.setSourcePath("C:\\Users\\erik\\Documents\\Visual Studio 2013\\Projects\\FluidSim\\FluidSim\\shaders\\basic.frag");

	m_TriangleProg = new Program;

	m_TriangleProg->attachShader(&vertSh);
	m_TriangleProg->attachShader(&fragSh);

	try {
		vertSh.compile();
		fragSh.compile();
		m_TriangleProg->link();
		m_TriangleProg->detachAllShaders();
		m_TriangleProg->use();
	}
	catch (const std::logic_error& err) {
		ERROR_MSG("Couldn't compile or link shader/program:\n" << err.what() << "\nTrying to cleanup...");
		try {
			cleanupTriangle();
		}
		catch (const std::exception& ex) {
			ERROR_MSG("Cleaning up triangle failed");
		}
		throw std::logic_error("Setting up triangle failed. Couldn't compile or link");
	}

	errorCheckVal = glGetError();
	if (errorCheckVal != GL_NO_ERROR) {
		ERROR_MSG("setting up triangle: opengl error:\n" << gluErrorString(errorCheckVal) << "\nTrying to cleanup...");
		//try to cleanup
		try {
			cleanupTriangle();
		}
		catch (const std::exception& ex) {
			ERROR_MSG("Cleanup triangle failed.");
		}
		throw std::exception("Setting up triangle failed: opengl error");
	}
}

void GLViewer::cleanup()
{
	try {
		GLViewer::deleteInstance();
	}
	catch (const std::exception& ex) {
		ERROR_MSG("In cleanup(): " << ex.what());
	}
	/*GLViewer *viewer = GLViewer::getInstance();
	if (viewer == nullptr) {
		throw std::runtime_error("in cleanup but instance of glviewer doesnt exist.");
	}
	try {
		viewer->cleanupTriangle();
	}
	catch (const std::exception& ex) {
		ERROR_MSG("In cleanup: " << ex.what());
	}*/
}

void GLViewer::cleanupTriangle()
{
	GLenum errorCheckVal = glGetError();

	delete m_TriangleProg;
	delete m_TriangleVao;
	delete m_TrianglePosVbo;
	delete m_TriangleColorVbo;

	errorCheckVal = glGetError();
	if (errorCheckVal != GL_NO_ERROR) {
		ERROR_MSG("Failed to cleanup triangle: opengl error:\n" << gluErrorString(errorCheckVal));
		throw std::exception("Cleanup triangle failed: opengl error");
	}
}

void GLViewer::drawTriangle()
{
	m_TriangleVao->bind();
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

