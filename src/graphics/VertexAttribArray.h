#pragma once

#include <GL/glew.h>
#include "VertexBufO.h"
class VertexAttribArray
{
public:
	VertexAttribArray(const VertexBufO& baseVbo, bool bindVboBefore, GLuint index, GLint componentsPerAttrib, GLenum componentType, GLboolean normalized, GLsizei stride, const GLvoid* offset);
	virtual ~VertexAttribArray();

	void enable();
	void disable();
private:
	const VertexBufO& m_BaseVbo;
	GLuint m_Index;
	GLint m_ComponentsPerAttrib;
	GLenum m_ComponentType;
	GLboolean m_Normalized;
	GLsizei m_Stride;
	const GLvoid* m_Offset;
	bool m_Enabled;
};

