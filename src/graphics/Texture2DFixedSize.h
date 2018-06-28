#pragma once

#include <GL/glew.h>

class Texture2DFixedSize
{
public:
	Texture2DFixedSize(GLenum internalFormat, size_t width, size_t height);
	virtual ~Texture2DFixedSize();

	void pushImage(GLenum format, GLenum type, const GLvoid *data);
	void texImage(GLenum format, GLenum type, GLvoid *img);
	const GLuint id() const;

	void bind();
	void unbind();

	void minFilter(GLint mode);
	void magFilter(GLint mode);

private:
		GLuint m_Id;
		size_t m_Width;
		size_t m_Height;
};

