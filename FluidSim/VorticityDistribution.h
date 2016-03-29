#pragma once

#include <glm/vec3.hpp>

class VorticityDistribution {
public:
	VorticityDistribution(const glm::vec3 &center);
	virtual ~VorticityDistribution();

	virtual glm::vec3 getVorticityAtPosition(const glm::vec3 &position) const = 0;
	glm::vec3 getDomainSize() const;
	glm::vec3 getMinCorner() const;

protected:
	virtual glm::vec3 calculateDomainSize() = 0;
	glm::vec3 getCenter() const;

private:
	glm::vec3 m_Center;
	glm::vec3 m_DomainSize;
};

class JetRingVorticityDistribution : public VorticityDistribution
{
public:
	JetRingVorticityDistribution(const glm::vec3 &center, float radiusSlug, float thickness, const glm::vec3 &direction);
	virtual ~JetRingVorticityDistribution();

	virtual glm::vec3 getVorticityAtPosition(const glm::vec3 &position) const override;

protected:
	virtual glm::vec3 calculateDomainSize() override;

private:
	float m_RadiusSlug;
	float m_RadiusOuter;
	float m_Thickness;
	glm::vec3 m_Direction;
};

