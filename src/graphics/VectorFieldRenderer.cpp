
#include "VectorFieldRenderer.h"
#include "../Scene.h"
#include "../util/Log.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/color_space.hpp>
#include <functional>

const double VectorFieldRenderer::m_MAX_VELOCITY = 5.0f;
const glm::dvec3 VectorFieldRenderer::m_INITIAL_DIRECTION = glm::dvec3(0, 0, -1);

VectorFieldRenderer::VectorFieldRenderer(Scene &scene, const grid_getter_t& gridGetter, Program &phongProgram)
	: m_VelocityGridGetter{ gridGetter }, m_GridResolutionFactor{ 1.0 }, m_GridResolutionChanged{true}, m_RenderArrows{true}, m_RenderLines{true}, m_LinesVerticesBuf{false}
{
	/*
	* Setup m_DrawPrototype
	*/
	Material arrowMaterial{ glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0.2f), 10 };
	Material &arrowMatRef = scene.addMaterial(arrowMaterial);

	//setup geometry
	Geometry &geomArrow = scene.addGeometryFromFile("objects\\arrow.obj");
	geomArrow.setupAttribArrays(phongProgram);

	//create vorton prototype
	m_DrawPrototype.material(&arrowMatRef);
	m_DrawPrototype.geometry(&geomArrow);
	m_DrawPrototype.program(&phongProgram);
	m_DrawPrototype.scale(glm::dvec3(0.05));

    setupLinesRenderProgram();
    setupLinesVao();
}


VectorFieldRenderer::~VectorFieldRenderer()
{
}

void VectorFieldRenderer::render(const glm::mat4x4 & viewProjectTransform)
{
	const auto& velocityGridPtr = m_VelocityGridGetter();
	if (!velocityGridPtr) {
		return;
	}

	glm::uvec3 expectedPointsAmount = glm::ceil(glm::vec3(velocityGridPtr->pointsAmount()) * m_GridResolutionFactor);
	size_t expectedCapacity = expectedPointsAmount.x * expectedPointsAmount.y * expectedPointsAmount.z;

	if (m_GridResolutionChanged || (m_CurrentArrows.size() != expectedCapacity)) {
		applyGridResolutionFactor();
	}

	glm::dvec3 currentPoint = velocityGridPtr->minCorner();
	size_t currentOffset = 0;

	glm::uvec3 pointsAmount = glm::ceil(glm::vec3(velocityGridPtr->pointsAmount()) * m_GridResolutionFactor);
	glm::dvec3 cellExtent = velocityGridPtr->gridExtent() / glm::dvec3(pointsAmount);

    std::vector<std::function<void()>> lineCreationFunctions;

    if (m_RenderLines) {
        lineCreationFunctions.resize(pointsAmount.x * pointsAmount.y * pointsAmount.z);
    }

	for (size_t x = 0; x < pointsAmount.x; x++) {
		for (size_t y = 0; y < pointsAmount.y; y++) {
			for (size_t z = 0; z < pointsAmount.z; z++) {
				render(viewProjectTransform, currentPoint, currentOffset, *velocityGridPtr, lineCreationFunctions);
				currentOffset++;
				currentPoint.z += cellExtent.z;
			}
			currentPoint.z = velocityGridPtr->minCorner().z;
			currentPoint.y += cellExtent.y;
		}
		currentPoint.y = velocityGridPtr->minCorner().y;
		currentPoint.x += cellExtent.x;
	}

    if (m_RenderLines) {
#pragma omp parallel for
        for (int i = 0; i < lineCreationFunctions.size(); i++) {
            lineCreationFunctions[i]();
        }
        renderLines(viewProjectTransform);
    }
}

void VectorFieldRenderer::gridResolutionFactor(float factor)
{
	m_GridResolutionFactor = factor;
	m_GridResolutionChanged = true;
}

void VectorFieldRenderer::renderLines(const bool shouldRender)
{
    m_RenderLines = shouldRender;
}

bool VectorFieldRenderer::isRenderingLines() const
{
    return m_RenderLines;
}

void VectorFieldRenderer::renderArrows(const bool shouldRender)
{
    m_RenderArrows = shouldRender;
}

bool VectorFieldRenderer::isRenderingArrows() const
{
    return m_RenderArrows;
}

void VectorFieldRenderer::applyGridResolutionFactor()
{
	m_GridResolutionChanged = false;

	const auto& velocityGridPtr = m_VelocityGridGetter();

	
	glm::uvec3 pointsAmount = glm::ceil(glm::vec3(velocityGridPtr->pointsAmount())*m_GridResolutionFactor);
	size_t capacity = pointsAmount.x * pointsAmount.y * pointsAmount.z;

	m_CurrentArrows.clear();
	m_CurrentArrows.reserve(capacity);
	for (int i = 0; i < capacity; i++) {
		m_CurrentArrows.emplace_back(glm::dvec3(0), 0.0);
	}

    static constexpr size_t componentsPerLineVertex = 6u;
    static constexpr size_t verticesPerLine = 2u;
    m_LineVerticesRAM.resize(capacity * componentsPerLineVertex * m_LineStepAmount * verticesPerLine);
}

void VectorFieldRenderer::render(const glm::mat4x4 & viewProjectTransform, const glm::dvec3 & pointPosition,
	size_t pointOffset, const UniformGrid<glm::dvec3>& velocityGrid,
    std::vector<std::function<void()>>& lineCreationFunctions)
{
    if (m_RenderArrows) {
        renderArrow(viewProjectTransform, pointPosition, pointOffset, velocityGrid);
    }
    if (m_RenderLines) {
        lineCreationFunctions[pointOffset] = [pointPosition, pointOffset, &velocityGrid, this]() {createLine(pointPosition, pointOffset, velocityGrid); };
    }
}

void VectorFieldRenderer::renderArrow(const glm::mat4x4 & viewProjectTransform, const glm::dvec3 & pointPosition, size_t pointOffset, const UniformGrid<glm::dvec3>& velocityGrid)
{
    const glm::dvec3 velocity = m_GridResolutionFactor == 1.0 ? velocityGrid.atOffset(pointOffset) : velocityGrid.interpolate(pointPosition);
    glm::dvec3 rgbColor = rgbColorFromVelocity(velocity);

    if (m_CurrentArrows[pointOffset].isHighlighted()) {
        rgbColor = glm::vec3(1.0, 0.0, 0.5);
    }

    m_DrawPrototype.position(pointPosition);

    m_DrawPrototype.material().ambientCoefficient(rgbColor);
    m_DrawPrototype.material().diffuseCoefficient(rgbColor);
    m_DrawPrototype.program().loadMaterial(m_DrawPrototype.material());

    glm::dvec3 velocityNormalized = glm::normalize(velocity);
    glm::vec3 rotationAxis = glm::cross(m_INITIAL_DIRECTION, velocityNormalized);
    float angle = glm::acos(glm::dot(velocityNormalized, m_INITIAL_DIRECTION));
    m_DrawPrototype.rotate(angle, rotationAxis);

    m_CurrentArrows[pointOffset].position(pointPosition);
    m_CurrentArrows[pointOffset].velocity(velocity);
    m_CurrentArrows[pointOffset].velocityNormalized(velocityNormalized);
    m_CurrentArrows[pointOffset].angle(angle);

    m_DrawPrototype.renderWithId(viewProjectTransform, m_CurrentArrows[pointOffset].id());

    m_DrawPrototype.resetRotation();
}

void VectorFieldRenderer::renderLines(const glm::mat4x4 & viewProjectTransform)
{
    static int64_t lastVertexBufferSize = -1;

    if (m_LineVerticesRAM.size() != lastVertexBufferSize) {
        m_LinesVerticesBuf.pushData(m_LineVerticesRAM, GL_DYNAMIC_DRAW, true);
        lastVertexBufferSize = m_LineVerticesRAM.size();
    } else {
        m_LinesVerticesBuf.pushDataSubset(m_LineVerticesRAM, 0, m_LineVerticesRAM.size(), true);
    }
    m_LinesVao.bind();
    m_LinesRenderProgram.use();
    m_LinesRenderProgram.loadModelViewProjectTransform(viewProjectTransform);

    glDrawArrays(GL_LINES, 0, m_LineVerticesRAM.size() / 6);

    m_LinesRenderProgram.unuse();
    m_LinesVao.unbind();

}

void VectorFieldRenderer::createLine(const glm::dvec3 & pointPosition, size_t pointOffset, const UniformGrid<glm::dvec3>& velocityGrid)
{
    assert(m_LineStepAmount > 0);

    static constexpr size_t verticesPerLine = 2u;
    const size_t lineOffset = pointOffset * m_LineStepAmount * verticesPerLine;

    glm::dvec3 position = pointPosition;

    glm::dvec3 velocity = velocityGrid.interpolate(position);
    glm::vec3 rgbColor = rgbColorFromVelocity(velocity);
    insertLinePoint(position, rgbColor, lineOffset);

    for (int step = 1; step < ((m_LineStepAmount-1)*2); step+=2) {
        const auto positionDelta = glm::normalize(velocity) * m_LineStepDistance;
        position += positionDelta;

        if (!fsmath::anyLess(position, velocityGrid.minCorner()) && !fsmath::anyLess(velocityGrid.minCorner() + velocityGrid.gridExtent(), position)) {
            velocity = velocityGrid.interpolate(position);
            rgbColor = rgbColorFromVelocity(velocity);
        } else {
            position -= positionDelta;
        }

        insertLinePoint(position, rgbColor, lineOffset + step);
        insertLinePoint(position, rgbColor, lineOffset + step + 1);
    }

    position += glm::normalize(velocity) * m_LineStepDistance;

    if (!fsmath::anyLess(position, velocityGrid.minCorner()) && !fsmath::anyLess(velocityGrid.minCorner() + velocityGrid.gridExtent(), position)) {
        velocity = velocityGrid.interpolate(position);
        rgbColor = rgbColorFromVelocity(velocity);
    }
    insertLinePoint(position, rgbColor, lineOffset + (m_LineStepAmount * verticesPerLine) - 1);

}

void VectorFieldRenderer::setupLinesRenderProgram()
{
    Shader vertexShader{ GL_VERTEX_SHADER };
    vertexShader.loadSourceFromFile("shaders\\vectorFieldLines.vert");

    Shader fragmentShader{ GL_FRAGMENT_SHADER };
    fragmentShader.loadSourceFromFile("shaders\\vectorFieldLines.frag");

    m_LinesRenderProgram.attachShader(&vertexShader);
    m_LinesRenderProgram.attachShader(&fragmentShader);

    vertexShader.compile();
    fragmentShader.compile();
    m_LinesRenderProgram.link();
    m_LinesRenderProgram.detachAllShaders();
}


void VectorFieldRenderer::setupLinesVao()
{
    m_LinesVao.addVertexAttribArray(m_LinesVerticesBuf, true, true, m_LinesRenderProgram.vertexPositionIndex(), 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, 0);
    m_LinesVao.enableVertexAttribArray(false, m_LinesRenderProgram.vertexPositionIndex());

    m_LinesVao.addVertexAttribArray(m_LinesVerticesBuf, false, false, 1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6,
        reinterpret_cast<const GLvoid*>(sizeof(GLfloat) * 3));
    m_LinesVao.enableVertexAttribArray(false, 1);
    m_LinesVao.unbind();
}

glm::vec3 VectorFieldRenderer::rgbColorFromVelocity(const glm::dvec3& velocity) const
{
    float velocityPowerfulness = static_cast<float>(glm::min(glm::length(velocity) / m_MAX_VELOCITY, 1.0));

    float hue = 240 - velocityPowerfulness * 240;
    glm::vec3 hsvColor = glm::vec3(hue, 0.8, 0.9);
    return glm::rgbColor(hsvColor);
}

void VectorFieldRenderer::insertLinePoint(const glm::dvec3 & position, const glm::vec3 & color, const size_t offset)
{
    static constexpr size_t componentsPerVertex = 6u;
    const size_t vectorOffset = offset * componentsPerVertex;
    for (size_t i = 0; i < position.length(); i++) {
        m_LineVerticesRAM[vectorOffset +i] = position[i];
    }
    for (size_t i = 0; i < color.length(); i++) {
        m_LineVerticesRAM[vectorOffset +position.length()+i] = color[i];
    }
}
