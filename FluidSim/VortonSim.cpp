
#include "VortonSim.h"
#include <glm/mat3x3.hpp>
#include "fsmath.h"

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
	return (m_VortonHeapPtr->getRoot().calculateVelocity(position));
}

void VortonSim::StretchAndTiltVortons(float seconds)
{
	UniformGrid<glm::mat3x3> velocityJacobianGrid(*m_VelocityGridPtr);
	fsmath::ComputeJacobian(velocityJacobianGrid, *m_VelocityGridPtr);

	for (auto &vorton : m_Vortons) {
		glm::mat3x3 velocityJacobian = velocityJacobianGrid.Interpolate(vorton.getPosition());

		glm::vec3 stretchTilt = vorton.getVorticity() * velocityJacobian;
		glm::vec3 newVorticity = vorton.getVorticity() + 0.5f * stretchTilt * seconds;	//0.5 is fudge factor for stability

		vorton.setVorticity(newVorticity);
	}
}

void VortonSim::DiffuseVorticityPSE(float seconds)
{
	for (auto leafs = m_VortonHeapPtr->getLeafs(); leafs.first != leafs.second; leafs.second++) {
		Supervorton &currentSupervorton = leafs.first->getSupervorton();
		DiffuseVorticityInside(seconds, currentSupervorton.getContainedVortons());

		for (auto &neighbor : leafs.first->getForwardNeighbors()) {
			Supervorton &neighborSupervorton = neighbor.getSupervorton();
			DiffuseVorticityBetween(seconds, currentSupervorton.getContainedVortons(), neighborSupervorton.getContainedVortons());
		}

		//dissipate vorticity
		for (auto vorton : currentSupervorton.getContainedVortons()) {
			vorton.setVorticity(vorton.getVorticity() * seconds * m_Viscosity);
		}
	}
}

void VortonSim::DiffuseVorticityInside(float seconds, std::vector<Vorton&>& vortons)
{
	for (auto outerIterator = vortons.begin(); outerIterator != vortons.end(); outerIterator++) {
		for (auto innerIterator = outerIterator + 1; innerIterator < vortons.end(); innerIterator++) {
			DiffuseVorticityBetween(seconds, *outerIterator, *innerIterator);
		}
	}
}

void VortonSim::DiffuseVorticityBetween(float seconds, std::vector<Vorton&>& firstVortons, std::vector<Vorton&>& secondVortons)
{
	for (auto outerIterator = firstVortons.begin(); outerIterator != firstVortons.end(); outerIterator++) {
		for (auto innerIterator = secondVortons.begin(); innerIterator != secondVortons.end(); innerIterator++) {
			DiffuseVorticityBetween(seconds, *outerIterator, *innerIterator);
		}
	}
}

void VortonSim::DiffuseVorticityBetween(float seconds, Vorton & first, Vorton & second)
{
	glm::vec3 vorticityDifference = first.getVorticity() - second.getVorticity();
	glm::vec3 exchange = 2 * m_Viscosity * seconds * vorticityDifference;
	first.setVorticity(first.getVorticity - exchange);
	second.setVorticity(second.getVorticity() + exchange);
}
