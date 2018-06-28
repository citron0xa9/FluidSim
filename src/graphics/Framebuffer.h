#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/vec2.hpp>
#include "Texture2DFixedSize.h"
#include "../Scene.h"

class Framebuffer
{
public:
	Framebuffer();
	virtual ~Framebuffer();

	void attachTexture(GLenum attachmentPoint, const Texture2DFixedSize &tex);
	void bind();
	void bindRead();
	void bindDraw();
	void unbind();
	void unbindRead();
	void unbindDraw();

	static void bindDefault();

	void addDrawBuffer(GLenum drawBuffer);
	void readBuffer(GLenum readBuffer);

	void blitColorToDefaultFBO(
		GLenum readBuffer, glm::ivec2 sourceStart, glm::ivec2 sourceEnd,
		glm::ivec2 destinationStart, glm::ivec2 destinationEnd
	);

	void render(Scene &scene);

	void readPixels(GLenum readBuffer, glm::ivec2 start, glm::ivec2 extent, GLenum format, GLenum type, GLvoid *data);

	bool isComplete();

private:
	GLuint m_Id;
	std::vector<GLenum> m_DrawBuffers;
};

