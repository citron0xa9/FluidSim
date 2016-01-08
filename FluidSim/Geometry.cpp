#include "Geometry.h"
#include "IndexCombination.h"
#include "StrUtils.h"
#include "Log.h"
#include <fstream>
#include <vector>
#include <glm/vec3.hpp>
#include <sstream>
#include <cstdlib>

Geometry::Geometry(const std::string& filePath) : m_VertexData{ false }, m_Ibo{ false }, m_Vao{false}
{

	/*
	**********************
	load geometry from file
	**********************
	*/

	//final arrays for vbo and ibo
	//std::vector<GLfloat> vertexData;
	//std::vector<GLushort> indices;

	try {
		parseObjFile(filePath, m_vertexDataRAM, m_indicesRAM);
	}
	catch (const std::exception& ex) {
		throw std::exception((std::string("Parsing .obj file failed: ") + ex.what()).c_str());
	}

	//push data to gpu
	m_VertexData.pushData(m_vertexDataRAM, GL_STATIC_DRAW, true);
	m_Ibo.pushData(m_indicesRAM, GL_STATIC_DRAW, true);

	m_numberOfElements = m_indicesRAM.size();
}

void Geometry::setupAttribArrays(Program &prog)
{
	m_Vao.addVertexAttribArray(m_VertexData, true, true, prog.getVertexPosIndex(), 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, 0);
	m_Vao.addVertexAttribArray(m_VertexData, false, true, prog.getNormalIndex(), 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, reinterpret_cast<const GLvoid*>(sizeof(GLfloat)*3) );
	m_Vao.enableVertexAttribArray(false, prog.getVertexPosIndex());
	m_Vao.enableVertexAttribArray(false, prog.getNormalIndex());
}

void Geometry::parseObjFile(const std::string filePath, std::vector<GLfloat>& vboData, std::vector<GLushort>& iboData)
{
	//temporary save values in arrays
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

	//save which data vbo already contains
	std::list<IndexCombination> combinations;
	//save next vbo index to use
	GLushort nextVboIndex = 0;

	//open file
	std::ifstream objFile;
	objFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	
	objFile.open(filePath);
	std::string line;
	while (!objFile.eof()) {
		try {
			std::getline(objFile, line);
		}
		catch (const std::exception &ex) {
			if (!objFile.eof()) {
				throw ex;
			}
			else {
				continue;
			}
		}
		std::istringstream lstream(line);
		std::string type;
		lstream >> type;
		if (type.compare("v") == 0) {
			parseVertex(lstream, vertices);
		}
		else if (type.compare("vn") == 0) {
			parseNormal(lstream, normals);
		}
		else if (type.compare("f") == 0) {
			parseFace(lstream, vboData, iboData, vertices, normals, &nextVboIndex, combinations);
		}
		else if (type.compare("s") == 0) {
			//ignore line
		}
		else if (type.compare("o") == 0) {
			//ignore line
		}
		else if (type.compare("#") == 0) {
			//ignore line
		}
		else {
			throw std::logic_error("Unknown start of line");
		}
	}
	objFile.close();
}

void Geometry::parseVertex(std::istringstream& lineStream, std::vector<glm::vec3>& vertices)
{
	glm::vec3 vertex;
	for (int i = 0; i < 3; i++) {
		lineStream >> vertex[i];
	}
	vertices.push_back(vertex);
}

void Geometry::parseNormal(std::istringstream& lineStream, std::vector<glm::vec3>& normals)
{
	glm::vec3 normal;
	for (int i = 0; i < 3; i++) {
		lineStream >> normal[i];
	}
	normals.push_back(normal);
}

void Geometry::parseFace(std::istringstream& lineStream, std::vector<GLfloat>& vboData, std::vector<GLushort>& iboData, std::vector<glm::vec3>& vertices,
	std::vector<glm::vec3>& normals, GLushort *nextVboIndex, std::list<IndexCombination>& combinations)
{
	for (int i = 0; i < 3; i++) {	//for every vertex
		std::string combinationStr;
		lineStream >> combinationStr;	//get combination as one string

		//parse it into the struct
		IndexCombination comb;
		std::vector<std::string> combinationParts = strutils::split(combinationStr, '/');
		comb.indices[0] = static_cast<GLushort>(atoi(combinationParts[0].c_str()))-1;
		comb.indices[1] = static_cast<GLushort>(atoi(combinationParts[2].c_str()))-1;

		//get pointer to this comb in the combinations vector + additional action if it is a new combination
		auto savedComb = std::find(combinations.begin(), combinations.end(), comb);
		if (savedComb == combinations.end()) {
			//is new comb

			//new ibo index
			comb.vboIndex = *nextVboIndex;
			(*nextVboIndex)++;

			//put combination into vbodata
			//vertex positions
			for (int i = 0; i < 3; i++) {
				vboData.push_back(vertices[comb.vertexIndex][i]);
			}
			//normal data
			for (int i = 0; i < 3; i++) {
				vboData.push_back(normals[comb.normalIndex][i]);
			}

			//save comb in combinations
			combinations.push_back(comb);

			//saved comb is this new comb
			savedComb = std::prev(combinations.end());
		}
		//write index for combination into ibo
		iboData.push_back(savedComb->vboIndex);
	}
}


Geometry::~Geometry()
{
}

void Geometry::render()
{
	m_VertexData.bind();
	m_Vao.bind();
	m_Ibo.bind();

	glDrawElements(GL_TRIANGLES, m_numberOfElements, GL_UNSIGNED_SHORT, nullptr);
}

void Geometry::debugRender(const glm::mat4x4 &mvpTransform) {
	DEBUG("\n==================================\nDEBUG RENDER: MODE: GL_TRIANGLES, going to render " << m_indicesRAM.size() / 3 << " triangles");
	for (int i = 0; i < m_indicesRAM.size(); i+=3) {
		DEBUG("+++++++++++++++++++++");
		DEBUG("Triangle " << i / 3 << ":");
		debugRenderTriangle(mvpTransform, i);

	}
}

void Geometry::debugRenderTriangle(const glm::mat4x4 &mvpTransform, int startIndex)
{
	DEBUG("\tLocalSpace:");
	for (int i = 0; i < 3; i++) {
		int vertIndex = m_indicesRAM[startIndex + i]*6;
		DEBUG("\tVertex " << i);
		DEBUG("\t\t (" << m_vertexDataRAM[vertIndex] << ", " << m_vertexDataRAM[vertIndex + 1] << ", " << m_vertexDataRAM[vertIndex + 2] << ")");
	}
	DEBUG("\tNormalized Device Coords:");
	
	for (int i = 0; i < 3; i++) {
		int vertIndex = m_indicesRAM[startIndex + i] * 6;
		DEBUG("\tVertex " << i);
		glm::vec4 vertTransformed = mvpTransform*glm::vec4(m_vertexDataRAM[vertIndex], m_vertexDataRAM[vertIndex + 1], m_vertexDataRAM[vertIndex + 2], 1.0);
		DEBUG("\t\t (" << vertTransformed.x/vertTransformed.w << ", " << vertTransformed.y/vertTransformed.w << ", " << vertTransformed.z/vertTransformed.w << ")");
	}
}