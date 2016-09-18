
#include "VorticityDistribution.h"
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>

VorticityDistribution::VorticityDistribution(const glm::dvec3 & center)
	: m_Center{center}
{
}

VorticityDistribution::~VorticityDistribution()
{
}

glm::dvec3 VorticityDistribution::minCorner() const
{
	return (m_Center - (m_DomainSize / 2.0));
}

glm::dvec3 VorticityDistribution::domainSize() const
{
	return m_DomainSize;
}

glm::dvec3 VorticityDistribution::center() const
{
	return m_Center;
}





JetRingVorticityDistribution::JetRingVorticityDistribution(const glm::dvec3 & center, double radiusSlug, double thickness, const glm::dvec3 & direction)
	: VorticityDistribution{ center }, m_RadiusSlug{ radiusSlug }, m_Thickness{ thickness }, m_RadiusOuter{ radiusSlug + thickness }, m_Direction{ direction }
{
	m_DomainSize = calculateDomainSize();
}

JetRingVorticityDistribution::~JetRingVorticityDistribution()
{
}

glm::dvec3 JetRingVorticityDistribution::vorticityAtPosition(const glm::dvec3 & position) const
{
	glm::dvec3 centerToPositionVec = position - center();
	double projectionOnAxis = glm::dot(centerToPositionVec, m_Direction);

	glm::dvec3 closestPointOnAxis = center() + m_Direction * projectionOnAxis;  // closest point on axis to vorton position
	glm::dvec3 outwardDirection = position - closestPointOnAxis;							// direction radially outward from annulus core
	double outwardDirectionLen = glm::length(outwardDirection);						// distance from axis

	if ((outwardDirectionLen < m_RadiusOuter) && (outwardDirectionLen > m_RadiusSlug))
	{   // Probe position is inside jet region.
		double streamwiseProfile = (abs(projectionOnAxis) < m_RadiusSlug) ? 0.5 * (cos( glm::pi<double>() * projectionOnAxis / m_RadiusSlug) + 1.0) : 0.0;
		double radialProfile = sin(glm::pi<double>() * (outwardDirectionLen - m_RadiusSlug) / m_Thickness);
		double vortPhi = streamwiseProfile * radialProfile * glm::pi<double>() / m_Thickness;
		glm::dvec3 outwardDirectionNormalized = glm::normalize(outwardDirection);
		glm::dvec3 phiHat = glm::cross(m_Direction, outwardDirectionNormalized);  // direction along annular core
		return (vortPhi * phiHat);
	}
	else
	{
		return glm::dvec3(0);
	}
}

glm::dvec3 JetRingVorticityDistribution::calculateDomainSize()
{
	return glm::dvec3(2*m_RadiusOuter);
}

VortexTubeVorticityDistribution::VortexTubeVorticityDistribution(const glm::dvec3 &center, double radius, double variation, double width, int periods, int location)
	: VorticityDistribution{ calculateCenter(center, location, radius) }, m_Radius{ radius }, m_Variation{ variation }, m_Width{ width },
	m_Wavenumber{ static_cast<double>(periods) }, m_Location{ location }
{
	m_DomainSize = calculateDomainSize();
}

VortexTubeVorticityDistribution::~VortexTubeVorticityDistribution()
{
}

glm::dvec3 VortexTubeVorticityDistribution::calculateDomainSize()
{
	return glm::dvec3(2.0 * m_Radius, m_Width, 2.0 * m_Radius);
}

glm::dvec3 VortexTubeVorticityDistribution::calculateCenter(const glm::dvec3 & center, int location, const double radius)
{
	if (location == 0) {
		return center;
	}
	if (location == 1) {
		return center - glm::dvec3(0, 0, radius);
	}
	if (location == -1) {
		return center - glm::dvec3(0, 0, -radius);
	}
	return center;
}

glm::dvec3 VortexTubeVorticityDistribution::vorticityAtPosition(const glm::dvec3 & position) const
{
	if (m_Location == 0 || m_Location == 1) {
		glm::dvec3 centerToPositionVec = position - center();
		const double distanceToMiddle = glm::length(glm::dvec2(centerToPositionVec.x, centerToPositionVec.z));
		const double modulation = 1.0 - m_Variation * (std::cos(2 * glm::pi<double>() * m_Wavenumber * centerToPositionVec.y / m_Width) - 1.0);
		const double localRadius = m_Radius * modulation;
		if (distanceToMiddle < localRadius)
		{   // Position is inside vortex tube.
			const double verticalVorticity = 0.5 * (std::cos(glm::pi<double>() * distanceToMiddle / localRadius) + 1);
			return glm::dvec3(0.0, verticalVorticity, 0.0);
		}
		else
		{   // Position is outside vortex tube.
			return glm::dvec3(0.0, 0.0, 0.0);
		}
	} else if (m_Location == -1) {
		glm::dvec3 centerToPositionVec = position - center();
		const double distanceToMiddle = glm::length(glm::dvec2(centerToPositionVec.y, centerToPositionVec.z));
		const double modulation = 1.0 - m_Variation * (std::cos(2 * glm::pi<double>() * m_Wavenumber * centerToPositionVec.x / m_Width) - 1.0);
		const double localRadius = m_Radius * modulation;
		if (distanceToMiddle < localRadius)
		{   // Position is inside vortex tube.
			const double xVorticity = 0.5 * (std::cos(glm::pi<double>() * distanceToMiddle / localRadius) + 1);
			return glm::dvec3(xVorticity, 0.0, 0.0);
		}
		else
		{   // Position is outside vortex tube.
			return glm::dvec3(0.0, 0.0, 0.0);
		}
	}
	else {
		throw std::invalid_argument("VorticityDistribution::VortexTube: Unknown location");
	}
}