
#include "VortonOctHeapRenderer.h"

#include <glm/gtc/type_ptr.hpp>

const GLuint VortonOctHeapRenderer::m_VertexPositionIndex = 0;
const std::string VortonOctHeapRenderer::m_ColorUniformName = "color";

VortonOctHeapRenderer::VortonOctHeapRenderer(std::function<const std::shared_ptr<VortonOctHeap>(void)> octHeapGetter)
	: m_LinePointsBuf{ false }, m_Color{ 1.0f, 1.0f, 0.0f }, m_OctHeapGetter{ octHeapGetter }, m_RenderedLevel{0}
{
	setupRendering();
}

VortonOctHeapRenderer::~VortonOctHeapRenderer()
{
}


void VortonOctHeapRenderer::render(const glm::mat4x4 & viewProjectTransform)
{
	const std::shared_ptr<VortonOctHeap> octHeap = m_OctHeapGetter();
	if (octHeap == nullptr) {
		return;
	}
	if (octHeap->divisions() < m_RenderedLevel) {
		std::string msg = "OctHeapRenderer: Divisions of oct heap is: " + std::to_string(octHeap->divisions()) + ".\n";
		msg += "Won't display level: " + std::to_string(m_RenderedLevel) + ".\n";
		msg += "Setting renderedLevel back to: " + std::to_string(octHeap->divisions()) + " and displaying this level.";
		WARNING(msg);
		m_RenderedLevel = octHeap->divisions();
	}
	
	updateGeometry(*octHeap);
	
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

void VortonOctHeapRenderer::setupRendering()
{
	setupVao();
	setupProgram();
}

void VortonOctHeapRenderer::updateGeometry(const VortonOctHeap &octHeap)
{
	m_LinePointsRAM.clear();
	
	glm::uvec3 pointsAmount = glm::uvec3((1 << m_RenderedLevel) + 1);
	size_t pointCapacity = pointsAmount.x * pointsAmount.y * pointsAmount.z;
	glm::dvec3 cellExtent = octHeap.extent() / glm::dvec3(pointsAmount-glm::uvec3(1));

	m_LinePointsRAM.reserve(18 * pointsAmount.x*pointsAmount.x);	//3*(2*x*y+2*x*z+2*y*z) with x=y=z, 3* because 3 components per point
	glm::dvec3 currentPoint;

	//push lines along z axis
	currentPoint = octHeap.minCorner();
	for (size_t x = 0; x < pointsAmount.x; x++) {
		for (size_t y = 0; y < pointsAmount.y; y++) {
			//push currentPoint
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}

			//push currentPoint with z offset by extent.z
			currentPoint.z += octHeap.extent().z;
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}
			currentPoint.z = octHeap.minCorner().z;

			currentPoint.y += cellExtent.y;
		}
		currentPoint.x += cellExtent.x;
		currentPoint.y = octHeap.minCorner().y;
	}

	//push lines along y axis
	currentPoint = octHeap.minCorner();
	for (size_t x = 0; x < pointsAmount.x; x++) {
		for (size_t z = 0; z < pointsAmount.z; z++) {
			//push currentPoint
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}

			//push currentPoint with y offset by extent.y
			currentPoint.y += octHeap.extent().y;
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}
			currentPoint.y = octHeap.minCorner().y;

			currentPoint.z += cellExtent.z;
		}
		currentPoint.x += cellExtent.x;
		currentPoint.z = octHeap.minCorner().z;
	}

	//push lines along x axis
	currentPoint = octHeap.minCorner();
	for (size_t y = 0; y < pointsAmount.y; y++) {
		for (size_t z = 0; z < pointsAmount.z; z++) {
			//push currentPoint
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}

			//push currentPoint with x offset by extent.x
			currentPoint.x += octHeap.extent().x;
			for (size_t i = 0; i < currentPoint.length(); i++) {
				m_LinePointsRAM.push_back(static_cast<GLfloat>(currentPoint[i]));
			}
			currentPoint.x = octHeap.minCorner().x;

			currentPoint.z += cellExtent.z;
		}
		currentPoint.y += cellExtent.y;
		currentPoint.z = octHeap.minCorner().z;
	}
}

void VortonOctHeapRenderer::renderedLevel(size_t level)
{
	m_RenderedLevel = level;
}

void VortonOctHeapRenderer::setupVao()
{
	m_LinesVao.addVertexAttribArray(m_LinePointsBuf, true, true, m_VertexPositionIndex, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr);
	m_LinesVao.enableVertexAttribArray(false, m_VertexPositionIndex);
}

void VortonOctHeapRenderer::setupProgram()
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
