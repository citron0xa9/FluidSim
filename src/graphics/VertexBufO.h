#pragma once

#include <GL/glew.h>
#include <vector>
#include "BufferO.h"

class VertexBufO : public BufferO
{
public:
	VertexBufO(bool ShouldBind);
	virtual ~VertexBufO();

	static void unbindStatic();
private:
	static const GLenum m_BUFFER_TYPE = GL_ARRAY_BUFFER;
	static const GLenum m_BUFFER_BINDING = GL_ARRAY_BUFFER_BINDING;
};

