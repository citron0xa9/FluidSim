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
	static GLViewer* getInstance();

	virtual ~GLViewer();
	static void MainLoop(void);

	void setWidth(int width);
	int getWidth() const;

	void setHeight(int height);
	int getHeight() const;

	std::string getTitlePrefix() const;

	void setTitle(const std::string& title);

	void setFrameCount(const unsigned int count);
	unsigned int getFrameCount() const;
	void incrementFrameCount();

	Scene& getScene();
private:
	GLViewer(const char* titlePrefix, unsigned int width, unsigned int height, int argc, char* argv[]);
	GLViewer(const GLViewer& v) {
		assert(0);	//shouldn't be called
	}

	void setupTriangle();
	void drawTriangle();

	void cleanupTriangle();
	static void cleanup();

	VertexArrO *m_TriangleVao;
	VertexBufO *m_TrianglePosVbo;
	VertexBufO *m_TriangleColorVbo;
	Program *m_TriangleProg;

	static void ResizeFunction(int width, int height);
	static void RenderFunction(void);
	static void IdleFunction(void);
	static void Timer_FPS(int value);

	unsigned int m_width, m_height;
	int m_WindowHandle;
	std::string m_title;
	std::string m_TitlePrefix;
	unsigned int m_FrameCount;
	const static unsigned int m_UPDATE_INTERVAL_MS = 250;

	static GLViewer* m_instance;

	Scene m_Scene;
};

