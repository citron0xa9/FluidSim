
#include "Vorton.h"

#include <glm/gtx/string_cast.hpp>

#include <iostream>

const double Vorton::ONE_OVER_4_PI = 1.0 / (4.0 * glm::pi<double>());
const double Vorton::VOLUME_CONSTANT = 8;
const double Vorton::VELOCITY_FROM_VORTICITY_CONSTANT = ONE_OVER_4_PI * VOLUME_CONSTANT;
const double Vorton::VORTICITY_FROM_VELOCITY_CONSTANT = 1.0 / VELOCITY_FROM_VORTICITY_CONSTANT;


Vorton::Vorton(const glm::dvec3 &initialPosition, const glm::dvec3 &vorticity, double radius, const double mass)
    : Tracer{ radius, mass }, m_Vorticity(vorticity)
{
	position(initialPosition);
}


Vorton::Vorton() : m_Vorticity{ 0 }
{
}

Vorton::~Vorton()
{
}

glm::dvec3 Vorton::inducedVelocity(const glm::dvec3 & querriedPosition) const
{
	if (glm::length(m_Vorticity) < SIGNIFICANT_VORTICITY) {
		return glm::vec3(0);
	}

    const double radiusPow2 = radius() * radius();
    const double radiusPow3 =  radiusPow2 * radius();

	glm::dvec3 distanceVector = querriedPosition - position();
	double distanceMagnitude = glm::length(distanceVector);
	double divisor = (distanceMagnitude < radius()) ? (radiusPow2 * distanceMagnitude) : (distanceMagnitude * distanceMagnitude * distanceMagnitude);
	return (VELOCITY_FROM_VORTICITY_CONSTANT * radiusPow3 * (glm::cross(m_Vorticity, distanceVector) / divisor));
}

void Vorton::printInfo() const
{
	std::string infoString;
	infoString += "Position: " + glm::to_string(position()) + "\nVorticity: " + glm::to_string(m_Vorticity) + "\n";
	INFO(infoString);
}

glm::dvec3 Vorton::vorticity() const
{
	return m_Vorticity;
}

void Vorton::vorticity(const glm::dvec3 & vorticity)
{
	m_Vorticity = vorticity;
}

void Vorton::vorticityByVelocity(const glm::dvec3 & velocity, const glm::dvec3 & positionOfVelocity)
{
    const auto vortonToPositionVector = positionOfVelocity - position();
    const double distance = glm::length(vortonToPositionVector);
    const double distancePow3 = distance * distance * distance;

    const double radiusPow3 = radius() * radius() * radius();

    const auto newVorticity = VORTICITY_FROM_VELOCITY_CONSTANT * distancePow3 * glm::cross(vortonToPositionVector, velocity) / (radiusPow3);

    vorticity(newVorticity);
}

Object * Vorton::copy() const
{
	return new Vorton(*this);
}

