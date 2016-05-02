#pragma once

#include <string>
#include <cassert>

#include "VertexArrO.h"
#include "VertexBufO.h"
#include "Program.h"
#include "Scene.h"

class GLViewer {
public:
	static void initInstance(const char* titlePrefix, unsigned int width, unsigned int height, int argc, char* argv[]);
	static void deleteInstance();
	static GLViewer* instance();

	virtual ~GLViewer();
	static void MainLoop(void);

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
private:
	GLViewer(const char* titlePrefix, unsigned int width, unsigned int height, int argc, char* argv[]);
	GLViewer(const GLViewer& v) = delete;

	static void cleanup();

	static void resizeFunction(int width, int height);
	static void renderFunction(void);
	static void idleFunction(void);
	static void timerFPS(int value);
	static void keyboardFunction(unsigned char key, int x, int y);
	static void keyboardFunctionUp(unsigned char key, int x, int y);
	static void mouseFunction(int button, int state, int x, int y);
	static void mouseMotionFunction(int x, int y);

	std::unordered_map<unsigned char, bool> m_KeysPressedState;

	unsigned int m_width, m_height;
	int m_WindowHandle;
	std::string m_title;
	std::string m_TitlePrefix;
	unsigned int m_FrameCount;
	const static unsigned int m_UPDATE_INTERVAL_MS = 250;

	static GLViewer* m_instance;

	Scene m_Scene;
	VortonSim *m_VortonSimPtr;

	bool m_MouseRotationReady;
	glm::vec2 m_LastMouseCoordinates;
	static const float m_MOUSE_TRANSLATION_TO_CAMERA_ROTATION;
};

