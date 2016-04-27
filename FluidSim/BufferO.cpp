
#include "BufferO.h"


BufferO::BufferO(GLenum bufferType) : m_bufferType{bufferType}
{
	glGenBuffers(1, &m_Id);
}


BufferO::~BufferO()
{
	glDeleteBuffers(1, &m_Id);
}

void BufferO::bind() const
{
	glBindBuffer(m_bufferType, m_Id);
}

void BufferO::unbind() const
{
	unbindStatic(m_bufferType);
}

void BufferO::unbindStatic(GLenum bufferType)
{
	glBindBuffer(bufferType, 0);
}

GLuint BufferO::id() const
{
	return m_Id;
}
