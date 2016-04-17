
#include "Supervorton.h"

/*Supervorton::Supervorton(const Supervorton & superVorton) : Vorton(superVorton), Object(superVorton)
{
}*/

Supervorton::Supervorton(Vorton & vorton) : Vorton(vorton), Object(vorton)
{
	setVorticity(glm::vec3(0));
	setPosition(glm::vec3(0));
	m_VorticityMagnitudeSum = 0;
}

/*Supervorton::Supervorton(const std::vector<Vorton*>& containedVortonPtrs)
	: Vorton{ calculateSupervorton(containedVortonPtrs) }, Object(*containedVortonPtrs.back()), m_ContainedVortonPtrs{ containedVortonPtrs }
{
}*/

Supervorton::~Supervorton()
{
}

void Supervorton::addContainedVorton(Vorton & vorton)
{
	m_ContainedVortonPtrs.push_back(&vorton);

	glm::vec3 position = getPosition();
	glm::vec3 vorticity = getVorticity();
	float vorticityMagnitude = static_cast<float>(glm::length(vorton.getVorticity()));

	position *= m_VorticityMagnitudeSum;
	position += vorticityMagnitude * vorton.getPosition();

	m_VorticityMagnitudeSum += vorticityMagnitude;

	vorticity += vorton.getVorticity();
	if (m_VorticityMagnitudeSum != 0) {
		position /= m_VorticityMagnitudeSum;

		setVorticity(vorticity);
		setPosition(position);
	}
}

std::vector<Vorton*>& Supervorton::getContainedVortonPtrs()
{
	return m_ContainedVortonPtrs;
}

void Supervorton::setContainedVortonPtrs(const std::vector<Vorton*>& containedVortons)
{
	m_ContainedVortonPtrs = containedVortons;
	Vorton &resultingVorton = calculateSupervorton(containedVortons);
	setVorticity(resultingVorton.getVorticity());
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

		float vorticityMagnitude = static_cast<float>(glm::length(vortonPtr->getVorticity()));
		m_VorticityMagnitudeSum += vorticityMagnitude;

		position += vorticityMagnitude * vortonPtr->getPosition();
		vorticity += vortonPtr->getVorticity();

	}
	if (m_VorticityMagnitudeSum > 0) {
		position /= m_VorticityMagnitudeSum;
	}
	else {
		position = containedVortonPtrs[0]->getPosition();
	}
	return Vorton(*containedVortonPtrs.back(), position, vorticity, containedVortonPtrs[0]->getRadius());
}
