
#include "Supervorton.h"

Supervorton::Supervorton() : m_VorticityMagnitudeSum{0}
{
}

Supervorton::Supervorton(const std::vector<Vorton&>& containedVortons) : m_ContainedVortons{containedVortons}, Vorton(calculateSupervorton(containedVortons))
{
}

Supervorton::~Supervorton()
{
}

void Supervorton::addContainedVorton(Vorton & vorton)
{
	m_ContainedVortons.push_back(vorton);

	glm::vec3 position = getPosition();
	glm::vec3 vorticity = getVorticity();
	float vorticityMagnitude = static_cast<float>(vorton.getVorticity().length());

	position *= m_VorticityMagnitudeSum;
	position += vorticityMagnitude * vorton.getPosition();

	m_VorticityMagnitudeSum += vorticityMagnitude;
	position /= m_VorticityMagnitudeSum;

	vorticity += vorton.getVorticity();

	setVorticity(vorticity);
	setPosition(position);
}

std::vector<Vorton&>& Supervorton::getContainedVortons()
{
	return m_ContainedVortons;
}

Vorton Supervorton::calculateSupervorton(const std::vector<Vorton&>& containedVortons)
{
	if (containedVortons.size() < 1) {
		std::runtime_error("Supervorton::calculateSupervorton: no contained vortons given");
	}
	glm::vec3 position;
	glm::vec3 vorticity;
	m_VorticityMagnitudeSum = 0;
	for (auto vorton : containedVortons) {

		float vorticityMagnitude = static_cast<float>(vorton.getVorticity().length());
		m_VorticityMagnitudeSum += vorticityMagnitude;

		position += vorticityMagnitude * vorton.getPosition();
		vorticity += vorton.getVorticity();

	}
	position /= m_VorticityMagnitudeSum;
	return Vorton(position, vorticity, containedVortons[0].getRadius());
}
