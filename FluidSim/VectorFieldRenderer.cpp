
#include "VectorFieldRenderer.h"
#include "Scene.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/color_space.hpp>

const double VectorFieldRenderer::m_MAX_VELOCITY = 5.0f;
const glm::dvec3 VectorFieldRenderer::m_INITIAL_DIRECTION = glm::dvec3(0, 0, -1);

VectorFieldRenderer::VectorFieldRenderer(Scene &scene, std::function<const std::shared_ptr<UniformGrid<glm::dvec3>>(void)> gridGetter, Program &phongProgram)
	: m_VelocityGridGetter{gridGetter}
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

	glm::dvec3 currentPoint = velocityGrid->minCorner();
	size_t currentOffset = 0;
	for (size_t x = 0; x < velocityGrid->pointsAmount().x; x++) {
		for (size_t y = 0; y < velocityGrid->pointsAmount().y; y++) {
			for (size_t z = 0; z < velocityGrid->pointsAmount().z; z++) {
				render(viewProjectTransform, currentPoint, currentOffset, *velocityGrid);
				currentOffset++;
				currentPoint.z += velocityGrid->cellExtent().z;
			}
			currentPoint.z = velocityGrid->minCorner().z;
			currentPoint.y += velocityGrid->cellExtent().y;
		}
		currentPoint.y = velocityGrid->minCorner().y;
		currentPoint.x += velocityGrid->cellExtent().x;
	}
}

void VectorFieldRenderer::render(const glm::mat4x4 & viewProjectTransform, const glm::dvec3 & pointPosition,
	size_t pointOffset, const UniformGrid<glm::dvec3>& velocityGrid)
{
	m_DrawPrototype.position(pointPosition);

	float velocityPowerfulness = static_cast<float>(glm::min(glm::length(velocityGrid.atOffset(pointOffset)) / m_MAX_VELOCITY, 1.0));

	float hue = 240 - velocityPowerfulness * 240;
	glm::vec3 hsvColor = glm::vec3(hue, 0.8, 0.9);
	glm::vec3 rgbColor = glm::rgbColor(hsvColor);

	m_DrawPrototype.material().ambientCoefficient(rgbColor);
	m_DrawPrototype.material().diffuseCoefficient(rgbColor);
	m_DrawPrototype.program().loadMaterial(m_DrawPrototype.material());

	glm::dvec3 velocityNormalized = glm::normalize(velocityGrid.atOffset(pointOffset));
	glm::vec3 rotationAxis = glm::cross(m_INITIAL_DIRECTION, velocityNormalized);
	float angle = glm::acos(glm::dot(velocityNormalized, m_INITIAL_DIRECTION));
	m_DrawPrototype.rotate(angle, rotationAxis);

	m_DrawPrototype.render(viewProjectTransform);

	m_DrawPrototype.resetRotation();
}

