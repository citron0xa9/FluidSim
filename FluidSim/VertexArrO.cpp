#include "VertexArrO.h"
#include "Log.h"


VertexArrO::VertexArrO(bool ShouldBind)
{
	glGenVertexArrays(1, &m_Id);
	if (ShouldBind) {
		bind();
	}
}


VertexArrO::~VertexArrO()
{
	//destroy all belonging VertexAttributeArrays
	clearVertexAttribArrays(true);

	/*
	//take care of unbinding if this vao is still bound
	GLint arrayBinding;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &arrayBinding);
	if (arrayBinding == m_Id) {
		DEBUG("Destroying bound VAO -> unbinding VAO");
		unbind();
	}
	*/

	//take care of unbinding
	unbind();

	//delete vao
	glDeleteVertexArrays(1, &m_Id);
}

void VertexArrO::bind() const
{
	glBindVertexArray(m_Id);
}

void VertexArrO::unbind()
{
	glBindVertexArray(0);
}

void VertexArrO::addVertexAttribArray(const VertexBufO& baseVbo, bool bindVaoBefore, bool bindVboBefore, GLuint index, GLint componentsPerAttrib, GLenum componentType, GLboolean normalized, GLsizei stride, const GLvoid *offset)
{
	if (bindVaoBefore) {
		bind();
	}
	m_VertexAttribArrays.insert(std::make_pair(index,new VertexAttribArray(baseVbo, bindVboBefore, index, componentsPerAttrib, componentType, normalized, stride, offset)));
}

void VertexArrO::enableVertexAttribArray(bool bindVaoBefore, GLuint index)
{
	if (bindVaoBefore) {
		bind();
	}
	m_VertexAttribArrays[index]->enable();
}

void VertexArrO::disableVertexAttribArray(bool bindVaoBefore, GLuint index)
{
	if (bindVaoBefore) {
		bind();
	}
	m_VertexAttribArrays[index]->disable();
}

void VertexArrO::clearVertexAttribArrays(bool bindVaoBefore)
{
	if (bindVaoBefore) {
		bind();
	}
	for (auto& elem : m_VertexAttribArrays) {
		delete elem.second;
	}
	m_VertexAttribArrays.clear();
}
