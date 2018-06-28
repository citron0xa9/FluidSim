
#include "Framebuffer.h"


Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &m_Id);
}


Framebuffer::~Framebuffer()
{
	unbind();
	glDeleteFramebuffers(1, &m_Id);
}

void Framebuffer::attachTexture(GLenum attachmentPoint, const Texture2DFixedSize & texture)
{
	bind();
	glFramebufferTexture(GL_FRAMEBUFFER, attachmentPoint, texture.id(), 0);
	unbind();
}

void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
}

void Framebuffer::bindRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Id);
}

void Framebuffer::bindDraw()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Id);
}

void Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::unbindRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void Framebuffer::unbindDraw()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Framebuffer::bindDefault()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::addDrawBuffer(GLenum drawBuffer)
{
	bindDraw();
	m_DrawBuffers.push_back(drawBuffer);
	glDrawBuffers(m_DrawBuffers.size(), m_DrawBuffers.data());
	unbindDraw();
}

void Framebuffer::readBuffer(GLenum readBuffer)
{
	bindRead();
	glReadBuffer(readBuffer);
	unbindRead();
}

void Framebuffer::blitColorToDefaultFBO(GLenum readBuffer, glm::ivec2 sourceStart, glm::ivec2 sourceEnd, glm::ivec2 destinationStart, glm::ivec2 destinationEnd)
{
	bindRead();
	glReadBuffer(readBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	
	glBlitFramebuffer(
		sourceStart.x, sourceStart.y, sourceEnd.x, sourceEnd.y,
		destinationStart.x, destinationStart.y, destinationEnd.x, destinationEnd.y,
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);

	unbindRead();
}

void Framebuffer::render(Scene & scene)
{
	bind();
	scene.render();
	unbind();
}

void Framebuffer::readPixels(GLenum readBuf, glm::ivec2 start, glm::ivec2 extent, GLenum format, GLenum type, GLvoid * data)
{
	readBuffer(readBuf);
	bindRead();
	glReadPixels(start.x, start.y, extent.x, extent.y, format, type, data);
	unbindRead();
}

bool Framebuffer::isComplete()
{
	bind();
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	unbind();
	return (result == GL_FRAMEBUFFER_COMPLETE);
}
