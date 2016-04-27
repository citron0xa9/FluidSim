
#include "IndexBufO.h"
#include "Log.h"

IndexBufO::IndexBufO(bool shouldBind) : BufferO(m_BUFFER_TYPE)
{
	if (shouldBind) {
		bind();
	}
}


IndexBufO::~IndexBufO()
{
	//take care of unbinding if this vbo is still bound
	GLint bufferBinding;
	glGetIntegerv(m_BUFFER_BINDING, &bufferBinding);
	if (bufferBinding == id()) {
		DEBUG("Destroying bound IBO -> unbinding IBO");
		unbind();
	}
}

void IndexBufO::unbindStatic()
{
	BufferO::unbindStatic(m_BUFFER_TYPE);
}
