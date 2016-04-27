#pragma once

#include <GL/glew.h>
#include <unordered_map>
#include "VertexBufO.h"
#include "VertexAttribArray.h"

class VertexArrO
{
public:
	VertexArrO(bool ShouldBind = true);
	virtual ~VertexArrO();

	void bind() const;
	static void unbind();

	void addVertexAttribArray(const VertexBufO& baseVbo, bool bindVaoBefore, bool bindVboBefore, GLuint index, GLint componentsPerAttrib, GLenum componentType, GLboolean normalized, GLsizei stride, const GLvoid *offset);
	void enableVertexAttribArray(bool bindVaoBefore, GLuint index);
	void disableVertexAttribArray(bool bindVaoBefore, GLuint index);

	void clearVertexAttribArrays(bool bindVaoBefore);
private:
	GLuint m_Id;

	std::unordered_map<GLuint, VertexAttribArray*> m_VertexAttribArrays; //vaa's are identified through their index
};

