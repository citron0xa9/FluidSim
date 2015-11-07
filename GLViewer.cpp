#include "GLViewer.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdexcept>

GLViewer::GLViewer(const char* title, unsigned int width, unsigned int height, int argc, char* argv[]) : m_width{ width }, m_height{ height }, m_title{ title } {
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	glutInitWindowSize(width, height);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	m_WindowHandle = glutCreateWindow(title);

	if (m_WindowHandle < 1) {
		throw std::runtime_error("Failed to create window");
	}

	glutReshapeFunc(&ResizeFunction);
	glutDisplayFunc(&RenderFunction);

	glClearColor(0.0f, 0.0f, 0.0f);
}

void GLViewer::ResizeFunction(int width, int height) {
	glViewport(0, 0, width, height);
	m_width = width;
	m_height = height;
}

void GLViewer::RenderFunction(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutSwapBuffers();
	glutPostRedisplay();
}

void GLViewer::MainLoop(void) {
	glutMainLoop();
}