
#include "Texture2DFixedSize.h"
#include <iostream>


Texture2DFixedSize::Texture2DFixedSize(GLenum internalFormat, size_t width, size_t height)
	: m_Width{width}, m_Height{height}
{
	glGenTextures(1, &m_Id);
	minFilter(GL_NEAREST);
	bind();
	glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
	unbind();
}

Texture2DFixedSize::~Texture2DFixedSize()
{
	unbind();
	glDeleteTextures(1, &m_Id);
}

void Texture2DFixedSize::pushImage(GLenum format, GLenum type, const GLvoid * data)
{
	bind();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, format, type, data);
	unbind();
}

const GLuint Texture2DFixedSize::id() const
{
	return m_Id;
}

void Texture2DFixedSize::bind()
{
	glBindTexture(GL_TEXTURE_2D, m_Id);
}

void Texture2DFixedSize::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2DFixedSize::minFilter(GLint mode)
{
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
	unbind();
}

void Texture2DFixedSize::magFilter(GLint mode)
{
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
	unbind();
}
