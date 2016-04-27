
#include "VorticityDistribution.h"
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>

VorticityDistribution::VorticityDistribution(const glm::vec3 & center)
	: m_Center{center}
{
}

VorticityDistribution::~VorticityDistribution()
{
}

glm::vec3 VorticityDistribution::minCorner() const
{
	return (m_Center - (m_DomainSize / 2.0f));
}

glm::vec3 VorticityDistribution::getDomainSize() const
{
	return m_DomainSize;
}

glm::vec3 VorticityDistribution::getCenter() const
{
	return m_Center;
}





JetRingVorticityDistribution::JetRingVorticityDistribution(const glm::vec3 & center, float radiusSlug, float thickness, const glm::vec3 & direction)
	: VorticityDistribution{ center }, m_RadiusSlug{ radiusSlug }, m_Thickness{ thickness }, m_RadiusOuter{ radiusSlug + thickness }, m_Direction{ direction }
{
	m_DomainSize = calculateDomainSize();
}

JetRingVorticityDistribution::~JetRingVorticityDistribution()
{
}

glm::vec3 JetRingVorticityDistribution::getVorticityAtPosition(const glm::vec3 & position) const
{
	glm::vec3 centerToPositionVec = position - getCenter();
	float projectionOnAxis = glm::dot(centerToPositionVec, m_Direction);

	glm::vec3 closestPointOnAxis = getCenter() + m_Direction * projectionOnAxis;  // closest point on axis to vorton position
	glm::vec3 outwardDirection = position - closestPointOnAxis;							// direction radially outward from annulus core
	float outwardDirectionLen = glm::length(outwardDirection);						// distance from axis

	if ((outwardDirectionLen < m_RadiusOuter) && (outwardDirectionLen > m_RadiusSlug))
	{   // Probe position is inside jet region.
		float streamwiseProfile = (fabsf(projectionOnAxis) < m_RadiusSlug) ? 0.5f * (cos( glm::pi<float>() * projectionOnAxis / m_RadiusSlug) + 1.0f) : 0.0f;
		float radialProfile = sin(glm::pi<float>() * (outwardDirectionLen - m_RadiusSlug) / m_Thickness);
		float vortPhi = streamwiseProfile * radialProfile * glm::pi<float>() / m_Thickness;
		glm::vec3 outwardDirectionNormalized = glm::normalize(outwardDirection);
		glm::vec3 phiHat = glm::cross(m_Direction, outwardDirectionNormalized);  // direction along annular core
		return (vortPhi * phiHat);
	}
	else
	{
		return glm::vec3(0);
	}
}

glm::vec3 JetRingVorticityDistribution::calculateDomainSize()
{
	return glm::vec3(2*m_RadiusOuter);
}
