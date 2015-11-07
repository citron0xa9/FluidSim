#include "VertexAttribArray.h"


VertexAttribArray::VertexAttribArray(const VertexBufO& baseVbo, bool bindVboBefore, GLuint index, GLint componentsPerAttrib, GLenum componentType, GLboolean normalized, GLsizei stride, const GLvoid* offset)
: m_BaseVbo{ baseVbo }, m_Index{ index }, m_ComponentsPerAttrib{ componentsPerAttrib }, m_ComponentType{ componentType }, m_Normalized{ normalized }, m_Stride{ stride }, m_Offset{ offset }, m_Enabled{ false }
{
	if (bindVboBefore) {
		m_BaseVbo.bind();
	}
	glVertexAttribPointer(index, componentsPerAttrib, componentType, normalized, stride, offset);
}


VertexAttribArray::~VertexAttribArray()
{
	if (m_Enabled) {
		disable();
	}
}

void VertexAttribArray::enable()
{
	glEnableVertexAttribArray(m_Index);
	m_Enabled = true;
}

void VertexAttribArray::disable()
{
	glDisableVertexAttribArray(m_Index);
	m_Enabled = false;
}
