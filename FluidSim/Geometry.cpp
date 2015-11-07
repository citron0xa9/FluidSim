#include "Geometry.h"
#include "IndexCombination.h"
#include "StrUtils.h"
#include <fstream>
#include <vector>
#include <glm/vec3.hpp>
#include <sstream>
#include <cstdlib>

Geometry::Geometry(const std::string& filePath) : m_VertexData{ false }, m_Ibo{false}
{

	/*
	**********************
	load geometry from file
	**********************
	*/

	//final arrays for vbo and ibo
	std::vector<GLfloat> vertexData;
	std::vector<GLushort> indices;

	try {
		parseObjFile(filePath, vertexData, indices);
	}
	catch (const std::exception& ex) {
		throw std::exception("Parsing .obj file failed: " << ex.what);
	}

	//push data to gpu
	m_VertexData.pushData(vertexData, GL_STATIC_DRAW, true);
	m_Ibo.pushData(indices, GL_STATIC_DRAW, true);
}

void Geometry::setupAttribArrays(VertexArrO& vao, GLuint vertexPosIndex, GLuint normalIndex)
{
	vao.addVertexAttribArray(m_VertexData, true, true, vertexPosIndex, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, 0);
	vao.addVertexAttribArray(m_VertexData, false, true, normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, reinterpret_cast<const GLvoid*>(sizeof(GLfloat)*3) );
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
		std::getline(objFile, line);
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
		comb.indices[0] = static_cast<GLushort>(atoi(combinationParts[0].c_str));
		comb.indices[1] = static_cast<GLushort>(atoi(combinationParts[2].c_str));

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
