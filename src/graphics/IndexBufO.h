#pragma once

#include "BufferO.h"

class IndexBufO : public BufferO
{
public:
	IndexBufO(bool shouldBind);
	virtual ~IndexBufO();

	static void unbindStatic();
private:
	static const GLenum m_BUFFER_TYPE = GL_ELEMENT_ARRAY_BUFFER;
	static const GLenum m_BUFFER_BINDING = GL_ELEMENT_ARRAY_BUFFER_BINDING;
};

