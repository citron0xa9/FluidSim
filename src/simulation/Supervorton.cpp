
#include "Supervorton.h"

Supervorton::Supervorton(const double birthTimeSeconds, double radius, const double mass)
    : Vorton{ birthTimeSeconds, radius, mass }, m_VorticityMagnitudeSum {0}
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
	calculateQuantitiesFromContainedVortons(containedVortons);
}

void Supervorton::calculateQuantitiesFromContainedVortons(const std::vector<Vorton*>& containedVortonPtrs)
{
	if (containedVortonPtrs.size() < 1) {
		std::runtime_error("Supervorton::calculateSupervorton: no contained vortons given");
	}
	glm::dvec3 calculatedPosition(0);
	glm::dvec3 calculatedVorticity(0);
	m_VorticityMagnitudeSum = 0;
	for (auto vortonPtr : containedVortonPtrs) {

		double vorticityMagnitude = glm::length(vortonPtr->vorticity());
		m_VorticityMagnitudeSum += vorticityMagnitude;

        calculatedPosition += vorticityMagnitude * vortonPtr->position();
        calculatedVorticity += vortonPtr->vorticity();

	}
	if (m_VorticityMagnitudeSum > 0) {
        calculatedPosition /= m_VorticityMagnitudeSum;
	}
	else {
        calculatedPosition = containedVortonPtrs[0]->position();
	}
    vorticity(calculatedVorticity);
    position(calculatedPosition);
}
