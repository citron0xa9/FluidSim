#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "VertexBufO.h"
#include "IndexBufO.h"
#include "VertexArrO.h"
#include "IndexCombination.h"
#include "Program.h"

#define GEOMETRY_IBO_INDEX_TYPE GL_UNSIGNED_SHORT

class Geometry
{
public:
	Geometry(const std::string& filePath);
	virtual ~Geometry();

	void setupAttribArrays(Program &prog);
	void render() const;

	void debugRender(const glm::mat4x4 &mvpTransform) const;
private:
	VertexBufO m_VertexData;
	IndexBufO m_Ibo;
	VertexArrO m_Vao;
	GLsizei m_numberOfElements;

	std::vector<GLfloat> m_vertexDataRAM;
	std::vector<GLushort> m_indicesRAM;

	void parseObjFile(const std::string filePath, std::vector<GLfloat>& vboData, std::vector<GLushort>& iboData);
	void parseVertex(std::istringstream& lineStream, std::vector<glm::vec3>& vertices);
	void parseNormal(std::istringstream& lineStream, std::vector<glm::vec3>& normals);
	void parseFace(std::istringstream& lineStream, std::vector<GLfloat>& vboData, std::vector<GLushort>& iboData,
		std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, GLushort *nextVboIndex, std::list<IndexCombination>& combinations);

	void debugRenderTriangle(const glm::mat4x4 &mvpTransform, int startIndex) const;
};

