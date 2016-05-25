
#include "Vorton.h"

Vorton::Vorton(const TriangleNetObject &triangleNetObject, const glm::dvec3 &initialPosition, const glm::dvec3 &vorticity, double radius)
	: TriangleNetObject(triangleNetObject), Object(triangleNetObject), m_Vorticity(vorticity), m_Radius(radius), m_Velocity(0)
{
	position(initialPosition);
}


Vorton::Vorton(const TriangleNetObject & triangleNetObj) : TriangleNetObject(triangleNetObj), Object(triangleNetObj), m_Vorticity{ 0 }
{
}

Vorton::~Vorton()
{
}

glm::dvec3 Vorton::inducedVelocity(const glm::dvec3 & querriedPosition) const
{
	/*if (m_Vorticity.length < SIGNIFICANT_VORTICITY) {
		return glm::vec3(0);
	}*/
	glm::dvec3 distanceVector = querriedPosition - position();
	double distanceMagnitude = glm::length(distanceVector);
	double divisor = (distanceMagnitude < m_Radius) ? (pow(m_Radius, 2) * distanceMagnitude) : pow(distanceMagnitude, 3);
	return glm::cross(ONE_OVER_FOUR_PI * 8.0f * pow(m_Radius, 3) * m_Vorticity, distanceVector / divisor);
}

glm::dvec3 Vorton::vorticity() const
{
	return m_Vorticity;
}

double Vorton::radius() const
{
	return m_Radius;
}

void Vorton::radius(double radius)
{
	m_Radius = radius;
}

void Vorton::vorticity(const glm::dvec3 & vorticity)
{
	m_Vorticity = vorticity;
}

Object * Vorton::copy() const
{
	return new Vorton(*this);
}

