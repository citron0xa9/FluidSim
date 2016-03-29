
#include "Vorton.h"


Vorton::Vorton() : m_Vorticity{0}
{
}

Vorton::Vorton(const glm::vec3 &position, const glm::vec3 &vorticity, float radius)
	: m_Position(position), m_Vorticity(vorticity), m_Radius(radius), m_Velocity(0)
{

}


Vorton::~Vorton()
{
}

glm::vec3 Vorton::inducedVelocity(const glm::vec3 & position) const
{
	/*if (m_Vorticity.length < SIGNIFICANT_VORTICITY) {
		return glm::vec3(0);
	}*/
	glm::vec3 distanceVector = position - m_Position;
	float distanceMagnitude = distanceVector.length();
	float divisor = (distanceMagnitude < m_Radius) ? pow(m_Radius, 3) : pow(distanceMagnitude, 3);
	return glm::cross(m_Vorticity, distanceVector) / divisor;
}

const glm::vec3 Vorton::getVorticity() const
{
	return m_Vorticity;
}

const glm::vec3 Vorton::getPosition() const
{
	return m_Position;
}

float Vorton::getRadius() const
{
	return m_Radius;
}

void Vorton::setVorticity(const glm::vec3 & vorticity)
{
	m_Vorticity = vorticity;
}

void Vorton::setPosition(const glm::vec3 & position)
{
	m_Position = position;
}
