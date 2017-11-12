#pragma once

#include <string>
#include <cassert>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <queue>
#include <functional>

#include "RigidBodySim.h"
#include "VortonSim.h"
#include "VertexArrO.h"
#include "VertexBufO.h"
#include "Program.h"
#include "Scene.h"
#include "VortonSimRenderer.h"
#include "Framebuffer.h"

class GLViewer {
public:
	static void initInstance(const char* titlePrefix, unsigned int width, unsigned int height);
	static void deleteInstance();
	static GLViewer* instance();

	virtual ~GLViewer();
	//static void MainLoop(void);

	void cycle();
	bool shouldClose() const;

	void width(int width);
	int width() const;

	void height(int height);
	int height() const;

	std::string titlePrefix() const;

	void title(const std::string& title);

	void frameCount(const unsigned int count);
	unsigned int frameCount() const;
	void incrementFrameCount();

	Scene& scene();
	VortonSim &vortonSim();
    RigidBodySim& rigidBodySim();
	VortonSimRenderer &vortonSimRenderer();

	void resetSim(bool createPaused);
private:
	GLViewer(const char* titlePrefix, unsigned int width, unsigned int height);
	GLViewer(const GLViewer& v) = delete;

	static void errorFunction(int error, const char *description);
	static void framebufferResizeFunction(GLFWwindow *windowPtr, int width, int height);
	//static void idleFunction(void);
	//static void timerFPS(int value);
	static void keyboardFunction(GLFWwindow *windowPtr, int key, int scancode, int action, int mods);
	void keyboardFunctionDown(int key);
	void keyboardFunctionUp(int key);

	static void mouseButtonFunction(GLFWwindow *windowPtr, int button, int action, int mods);
	static void mouseMotionFunction(GLFWwindow *windowPtr, double x, double y);

	void setupSim(bool createPaused);
	void attachTexturesToFBO();

	void handleRightclick(glm::dvec2 &cursorPosition);

	std::unordered_map<unsigned char, bool> m_KeysPressedState;
	std::queue<std::function<void()>> m_PerformAfterRender;

	unsigned int m_width, m_height;
	GLFWwindow *m_WindowPtr;
	std::string m_title;
	std::string m_TitlePrefix;
	unsigned int m_FrameCount;
	const static unsigned int m_UPDATE_INTERVAL_MS = 250;

	static GLViewer* m_instance;

	Scene m_Scene;
	std::unique_ptr<Framebuffer> m_MainFramebufferPtr;
	std::unique_ptr<Texture2DFixedSize> m_MainColorTexturePtr;
	std::unique_ptr<Texture2DFixedSize> m_ObjectIndexTexturePtr;
	std::unique_ptr<Texture2DFixedSize> m_MainDepthTexturePtr;

	VortonSim *m_VortonSimPtr;
	VortonSimRenderer *m_VortonSimRendererPtr;
    RigidBodySim* m_RigidBodySimPtr;

	bool m_MouseRotationReady;
	glm::vec2 m_LastMouseCoordinates;
	static const float m_MOUSE_TRANSLATION_TO_CAMERA_ROTATION;
};

