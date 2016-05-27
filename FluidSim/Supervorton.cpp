
#include "Supervorton.h"

Supervorton::Supervorton(TriangleNetObject & object) : Vorton(object), m_VorticityMagnitudeSum{0}
{
}

Supervorton::~Supervorton()
{
}

void Supervorton::addContainedVorton(Vorton & vorton)
{
	m_ContainedVortonPtrs.push_back(&vorton);

	glm::dvec3 currentPosition = position();
	glm::dvec3 currentVorticity = vorticity();
	double vorticityMagnitude = glm::length(vorton.vorticity());

	currentPosition *= m_VorticityMagnitudeSum;
	currentPosition += vorticityMagnitude * vorton.position();

	m_VorticityMagnitudeSum += vorticityMagnitude;

	currentVorticity += vorton.vorticity();
	if (m_VorticityMagnitudeSum != 0) {
		currentPosition /= m_VorticityMagnitudeSum;

		vorticity(currentVorticity);
		position(currentPosition);
	}
}

std::vector<Vorton*>& Supervorton::containedVortonPtrs()
{
	return m_ContainedVortonPtrs;
}

void Supervorton::containedVortonPtrs(const std::vector<Vorton*>& containedVortons)
{
	m_ContainedVortonPtrs = containedVortons;
	Vorton &resultingVorton = calculateSupervorton(containedVortons);
	vorticity(resultingVorton.vorticity());
	position(resultingVorton.position());
}

Vorton Supervorton::calculateSupervorton(const std::vector<Vorton*>& containedVortonPtrs)
{
	if (containedVortonPtrs.size() < 1) {
		std::runtime_error("Supervorton::calculateSupervorton: no contained vortons given");
	}
	glm::dvec3 position(0);
	glm::dvec3 vorticity(0);
	m_VorticityMagnitudeSum = 0;
	for (auto vortonPtr : containedVortonPtrs) {

		double vorticityMagnitude = glm::length(vortonPtr->vorticity());
		m_VorticityMagnitudeSum += vorticityMagnitude;

		position += vorticityMagnitude * vortonPtr->position();
		vorticity += vortonPtr->vorticity();

	}
	if (m_VorticityMagnitudeSum > 0) {
		position /= m_VorticityMagnitudeSum;
	}
	else {
		position = containedVortonPtrs[0]->position();
	}
	return Vorton(*containedVortonPtrs.back(), position, vorticity, containedVortonPtrs[0]->radius());
}
