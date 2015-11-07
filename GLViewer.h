#pragma once

#include <cstring>

class GLViewer {
public:
	GLViewer(const char* title, unsigned int width, unsigned int height, int argc, char* argv[]) : m_width{ width }, m_height{ height }, m_title{ title };
	virtual ~GLViewer(void) {}
	void MainLoop(void);
private:
	void ResizeFunction(int width, int height);
	void RenderFunction(void);

	unsigned int m_width, m_height;
	int m_WindowHandle;
	std::string m_title;
};