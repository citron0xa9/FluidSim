
#include "Supervorton.h"

Supervorton::Supervorton(TriangleNetObject & object) : Vorton(object), Object(object), m_VorticityMagnitudeSum{0}
{
}

Supervorton::~Supervorton()
{
}

void Supervorton::addContainedVorton(Vorton & vorton)
{
	m_ContainedVortonPtrs.push_back(&vorton);

	glm::vec3 position = getPosition();
	glm::vec3 vorticity = vorticity();
	float vorticityMagnitude = static_cast<float>(glm::length(vorton.vorticity()));

	position *= m_VorticityMagnitudeSum;
	position += vorticityMagnitude * vorton.getPosition();

	m_VorticityMagnitudeSum += vorticityMagnitude;

	vorticity += vorton.vorticity();
	if (m_VorticityMagnitudeSum != 0) {
		position /= m_VorticityMagnitudeSum;

		vorticity(vorticity);
		setPosition(position);
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
	setPosition(resultingVorton.getPosition());
}

Vorton Supervorton::calculateSupervorton(const std::vector<Vorton*>& containedVortonPtrs)
{
	if (containedVortonPtrs.size() < 1) {
		std::runtime_error("Supervorton::calculateSupervorton: no contained vortons given");
	}
	glm::vec3 position(0);
	glm::vec3 vorticity(0);
	m_VorticityMagnitudeSum = 0;
	for (auto vortonPtr : containedVortonPtrs) {

		float vorticityMagnitude = static_cast<float>(glm::length(vortonPtr->vorticity()));
		m_VorticityMagnitudeSum += vorticityMagnitude;

		position += vorticityMagnitude * vortonPtr->getPosition();
		vorticity += vortonPtr->vorticity();

	}
	if (m_VorticityMagnitudeSum > 0) {
		position /= m_VorticityMagnitudeSum;
	}
	else {
		position = containedVortonPtrs[0]->getPosition();
	}
	return Vorton(*containedVortonPtrs.back(), position, vorticity, containedVortonPtrs[0]->radius());
}
