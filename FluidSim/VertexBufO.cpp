#include "VertexBufO.h"
#include <vector>
#include "Log.h"


VertexBufO::VertexBufO(bool ShouldBind) : BufferO(m_BUFFER_TYPE)
{
	if (ShouldBind) {
		bind();
	}
}


VertexBufO::~VertexBufO()
{
	//take care of unbinding if this vbo is still bound
	GLint bufferBinding;
	glGetIntegerv(m_BUFFER_BINDING, &bufferBinding);
	if (bufferBinding == id()) {
		DEBUG("Destroying bound VBO -> unbinding VBO");
		unbind();
	}

}

void VertexBufO::unbindStatic()
{
	BufferO::unbindStatic(m_BUFFER_TYPE);
}


