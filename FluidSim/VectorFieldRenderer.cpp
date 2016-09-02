
#include "VectorFieldRenderer.h"
#include "Scene.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/color_space.hpp>

const double VectorFieldRenderer::m_MAX_VELOCITY = 5.0f;
const glm::dvec3 VectorFieldRenderer::m_INITIAL_DIRECTION = glm::dvec3(0, 0, -1);

VectorFieldRenderer::VectorFieldRenderer(Scene &scene, std::function<const std::shared_ptr<UniformGrid<glm::dvec3>>(void)> gridGetter, Program &phongProgram)
	: m_VelocityGridGetter{ gridGetter }, m_GridResolutionFactor{ 1.0 }, m_GridResolutionChanged{true}
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
}


VectorFieldRenderer::~VectorFieldRenderer()
{
}

void VectorFieldRenderer::render(const glm::mat4x4 & viewProjectTransform)
{
	const std::shared_ptr<UniformGrid<glm::dvec3>> velocityGrid = m_VelocityGridGetter();
	if (velocityGrid == nullptr) {
		return;
	}

	glm::uvec3 expectedPointsAmount = glm::ceil(glm::vec3(velocityGrid->pointsAmount()) * m_GridResolutionFactor);
	size_t expectedCapacity = expectedPointsAmount.x * expectedPointsAmount.y * expectedPointsAmount.z;

	if (m_GridResolutionChanged || (m_CurrentArrows.size() != expectedCapacity)) {
		applyGridResolutionFactor();
	}

	glm::dvec3 currentPoint = velocityGrid->minCorner();
	size_t currentOffset = 0;

	glm::uvec3 pointsAmount = glm::ceil(glm::vec3(velocityGrid->pointsAmount()) * m_GridResolutionFactor);
	glm::dvec3 cellExtent = velocityGrid->gridExtent() / glm::dvec3(pointsAmount);

	for (size_t x = 0; x < pointsAmount.x; x++) {
		for (size_t y = 0; y < pointsAmount.y; y++) {
			for (size_t z = 0; z < pointsAmount.z; z++) {
				render(viewProjectTransform, currentPoint, currentOffset, *velocityGrid);
				currentOffset++;
				currentPoint.z += cellExtent.z;
			}
			currentPoint.z = velocityGrid->minCorner().z;
			currentPoint.y += cellExtent.y;
		}
		currentPoint.y = velocityGrid->minCorner().y;
		currentPoint.x += cellExtent.x;
	}
}

void VectorFieldRenderer::gridResolutionFactor(float factor)
{
	m_GridResolutionFactor = factor;
	m_GridResolutionChanged = true;
}

void VectorFieldRenderer::applyGridResolutionFactor()
{
	m_GridResolutionChanged = false;

	const std::shared_ptr<UniformGrid<glm::dvec3>> velocityGrid = m_VelocityGridGetter();

	
	glm::uvec3 pointsAmount = glm::ceil(glm::vec3(velocityGrid->pointsAmount())*m_GridResolutionFactor);
	size_t capacity = pointsAmount.x * pointsAmount.y * pointsAmount.z;

	m_CurrentArrows.clear();
	m_CurrentArrows.reserve(capacity);
	for (int i = 0; i < capacity; i++) {
		m_CurrentArrows.emplace_back(glm::dvec3(0), 0.0);
	}
}

void VectorFieldRenderer::render(const glm::mat4x4 & viewProjectTransform, const glm::dvec3 & pointPosition,
	size_t pointOffset, const UniformGrid<glm::dvec3>& velocityGrid)
{
	m_DrawPrototype.position(pointPosition);

	const glm::dvec3 velocity = m_GridResolutionFactor == 1.0 ? velocityGrid.atOffset(pointOffset) : velocityGrid.interpolate(pointPosition);
	float velocityPowerfulness = static_cast<float>(glm::min(glm::length(velocity) / m_MAX_VELOCITY, 1.0));

	float hue = 240 - velocityPowerfulness * 240;
	glm::vec3 hsvColor = glm::vec3(hue, 0.8, 0.9);
	glm::vec3 rgbColor = glm::rgbColor(hsvColor);

	if (m_CurrentArrows[pointOffset].isHighlighted()) {
		rgbColor = glm::vec3(1.0, 0.0, 0.5);
	}

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

