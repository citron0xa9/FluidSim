
#include "Vorton.h"

Vorton::Vorton(const TriangleNetObject &triangleNetObject, const glm::vec3 &position, const glm::vec3 &vorticity, float radius)
	: TriangleNetObject(triangleNetObject), Object(triangleNetObject), m_Vorticity(vorticity), m_Radius(radius), m_Velocity(0)
{
	setPosition(position);
}


Vorton::Vorton(const TriangleNetObject & triangleNetObj) : TriangleNetObject(triangleNetObj), Object(triangleNetObj), m_Vorticity{ 0 }
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
	glm::vec3 distanceVector = position - getPosition();
	float distanceMagnitude = static_cast<float>(distanceVector.length());
	float divisor = (distanceMagnitude < m_Radius) ? pow(m_Radius, 3) : pow(distanceMagnitude, 3);
	return glm::cross(m_Vorticity, distanceVector) / divisor;
}

glm::vec3 Vorton::getVorticity() const
{
	return m_Vorticity;
}

float Vorton::getRadius() const
{
	return m_Radius;
}

void Vorton::setVorticity(const glm::vec3 & vorticity)
{
	m_Vorticity = vorticity;
}

Object * Vorton::copy() const
{
	return new Vorton(*this);
}

