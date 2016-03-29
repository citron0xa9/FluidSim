
#include "VortonSim.h"

const size_t VortonSim::m_VORTONS_PER_DIMENSION = 16;

VortonSim::VortonSim(float viscosity, float density, const VorticityDistribution &initialVorticity, float vorticityMagnitude)
	: m_Viscosity{viscosity}, m_Density{density}, m_VortonHeapPtr{nullptr}, m_VelocityGridPtr{nullptr}
{
	glm::vec3 minCorner = initialVorticity.getMinCorner();
	glm::vec3 maxCorner = minCorner + initialVorticity.getDomainSize();
	glm::vec3 vortonDistance = initialVorticity.getDomainSize() / static_cast<float>(m_VORTONS_PER_DIMENSION);
	for (int xIndex = 0; xIndex < m_VORTONS_PER_DIMENSION; xIndex++) {
		for (int yIndex = 0; yIndex < m_VORTONS_PER_DIMENSION; yIndex++) {
			for (int zIndex = 0; zIndex < m_VORTONS_PER_DIMENSION; zIndex++) {
				glm::vec3 position = glm::vec3(
					vortonDistance.x * xIndex,
					vortonDistance.y * yIndex,
					vortonDistance.z * zIndex);

				Vorton vorton(position, initialVorticity.getVorticityAtPosition(position) * vorticityMagnitude);
				m_Vortons.push_back(vorton);
			}
		}
	}
}

VortonSim::~VortonSim()
{
	if (m_VortonHeapPtr != nullptr) {
		delete m_VortonHeapPtr;
	}
	if (m_VelocityGridPtr != nullptr) {
		delete m_VelocityGridPtr;
	}
}

void VortonSim::Update(float seconds)
{
	CreateOctHeap();
	
	CalculateVelocityGrid();
}

void VortonSim::CreateOctHeap()
{
	if (m_VortonHeapPtr != nullptr) {
		delete m_VortonHeapPtr;
	}
	m_VortonHeapPtr = new VortonOctHeap(m_Vortons);
}

void VortonSim::CalculateVelocityGrid()
{
	size_t numPoints = m_VORTONS_PER_DIMENSION * m_VORTONS_PER_DIMENSION * m_VORTONS_PER_DIMENSION;
	if (m_VelocityGridPtr != nullptr) {
		delete m_VelocityGridPtr;
	}
	m_VelocityGridPtr = new UniformGrid<glm::vec3>(numPoints, m_VortonHeapPtr->getMinCorner(), m_VortonHeapPtr->getMinCorner() + m_VortonHeapPtr->getExtent(), false);
	
	//loop over every point in uniform grid
	size_t velocityGridOffset = 0;
	glm::vec3 currentGridPosition = m_VelocityGridPtr->GetMinCorner();

	for (size_t zIndex = 0; zIndex < m_VelocityGridPtr->GetPointsAmount().z; zIndex++) {
		for (size_t yIndex = 0; yIndex < m_VelocityGridPtr->GetPointsAmount().y; yIndex++) {
			for (size_t xIndex = 0; xIndex < m_VelocityGridPtr->GetPointsAmount().x; xIndex++) {
				m_VelocityGridPtr->AtOffset(velocityGridOffset) = CalculateVelocity(currentGridPosition);
				velocityGridOffset++;
				currentGridPosition.x += m_VelocityGridPtr->GetCellExtent().x;
			}
			currentGridPosition.x = m_VelocityGridPtr->GetMinCorner().x;
			currentGridPosition.y += m_VelocityGridPtr->GetCellExtent().y;
		}
		currentGridPosition.y = m_VelocityGridPtr->GetMinCorner().y;
		currentGridPosition.z += m_VelocityGridPtr->GetCellExtent().z;
	}
}

glm::vec3 VortonSim::CalculateVelocity(const glm::vec3 & position)
{
	assert(m_VortonHeapPtr != nullptr);
	return m_VortonHeapPtr->getRoot().calculateVelocity(position);
}
