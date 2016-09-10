
#include "GridRenderer.h"
#include <glm/gtc/type_ptr.hpp>

const GLuint GridRenderer::m_VertexPositionIndex = 0;
const std::string GridRenderer::m_ColorUniformName = "color";

GridRenderer::GridRenderer(std::function<const std::shared_ptr<UniformGridGeometry>(void)> gridGeometryGetter)
	: m_LinePointsBuf{ false }, m_Color{ 0.345f, 0.949f, 0.122f }, m_GridGeometryGetter{gridGeometryGetter}
{
	setupRendering();
}

GridRenderer::~GridRenderer()
{
}

void GridRenderer::render(const glm::mat4x4 & viewProjectTransform)
{
	const std::shared_ptr<UniformGridGeometry> gridGeometry = m_GridGeometryGetter();
	if (gridGeometry == nullptr) {
		return;
	}
	
	updateGeometry(*gridGeometry);
	m_LinePointsBuf.pushData(m_LinePointsRAM, GL_STATIC_DRAW, true);
	
	m_Program.use();

	GLuint colorUniformLocation = m_Program.uniformLocation(m_ColorUniformName.c_str());
	glUniform3fv(colorUniformLocation, 1, glm::value_ptr(m_Color));
	m_Program.loadModelViewProjectTransform(viewProjectTransform);

	m_LinesVao.bind();

	glDrawArrays(GL_LINES, 0, m_LinePointsRAM.size() / 3);

	m_LinesVao.unbind();
	m_Program.unuse();
}

void GridRenderer::setupRendering()
{
	setupVao();
	setupProgram();
}

void GridRenderer::updateGeometry(const UniformGridGeometry &geometry)
{
	m_LinePointsRAM.clear();
	m_LinePointsRAM.reserve(18 * geometry.pointsAmount().x*geometry.pointsAmount().x); //3*(2*x*y+2*x*z+2*y*z) with x=y=z, 3* because 3 components per point
	glm::dvec3 currentPoint;

	//push lines along z axis
	currentPoint = geometry.minCorner();
	for (size_t x = 0; x < geometry.pointsAmount().x; x++) {
		for (size_t y = 0; y < geometry.pointsAmount().y; y++) {
			//push currentPoint
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}

			//push currentPoint with z offset by gridExtent.z
			currentPoint.z += geometry.gridExtent().z;
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}
			currentPoint.z = geometry.minCorner().z;

			currentPoint.y += geometry.cellExtent().y;
		}
		currentPoint.x += geometry.cellExtent().x;
		currentPoint.y = geometry.minCorner().y;
	}

	//push lines along y axis
	currentPoint = geometry.minCorner();
	for (size_t x = 0; x < geometry.pointsAmount().x; x++) {
		for (size_t z = 0; z < geometry.pointsAmount().z; z++) {
			//push currentPoint
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}

			//push currentPoint with y offset by gridExtent.y
			currentPoint.y += geometry.gridExtent().y;
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}
			currentPoint.y = geometry.minCorner().y;

			currentPoint.z += geometry.cellExtent().z;
		}
		currentPoint.x += geometry.cellExtent().x;
		currentPoint.z = geometry.minCorner().z;
	}

	//push lines along x axis
	currentPoint = geometry.minCorner();
	for (size_t y = 0; y < geometry.pointsAmount().y; y++) {
		for (size_t z = 0; z < geometry.pointsAmount().z; z++) {
			//push currentPoint
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}

			//push currentPoint with x offset by gridExtent.x
			currentPoint.x += geometry.gridExtent().x;
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}
			currentPoint.x = geometry.minCorner().x;

			currentPoint.z += geometry.cellExtent().z;
		}
		currentPoint.y += geometry.cellExtent().y;
		currentPoint.z = geometry.minCorner().z;
	}
}

void GridRenderer::setupVao()
{
	m_LinesVao.addVertexAttribArray(m_LinePointsBuf, true, true, m_VertexPositionIndex, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr);
	m_LinesVao.enableVertexAttribArray(false, m_VertexPositionIndex);
}

void GridRenderer::setupProgram()
{
	Shader vertexShader{ GL_VERTEX_SHADER };
	vertexShader.loadSourceFromFile("shaders\\grid.vert");

	Shader fragmentShader{ GL_FRAGMENT_SHADER };
	fragmentShader.loadSourceFromFile("shaders\\grid.frag");

	m_Program.attachShader(&vertexShader);
	m_Program.attachShader(&fragmentShader);

	vertexShader.compile();
	fragmentShader.compile();
	m_Program.link();
	m_Program.detachAllShaders();
}
