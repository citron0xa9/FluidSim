#pragma once

#include <glm/vec3.hpp>

class VorticityDistribution {
public:
	VorticityDistribution(const glm::dvec3 &center);
	virtual ~VorticityDistribution();

	virtual glm::dvec3 vorticityAtPosition(const glm::dvec3 &position) const = 0;
	glm::dvec3 minCorner() const;

	glm::dvec3 domainSize() const;

protected:
	virtual glm::dvec3 calculateDomainSize() = 0;
	glm::dvec3 center() const;
	glm::dvec3 m_DomainSize;

private:
	glm::dvec3 m_Center;
	
};

class JetRingVorticityDistribution : public VorticityDistribution
{
public:
	JetRingVorticityDistribution(const glm::dvec3 &center, double radiusSlug, double thickness, const glm::dvec3 &direction);
	virtual ~JetRingVorticityDistribution();

	virtual glm::dvec3 vorticityAtPosition(const glm::dvec3 &position) const override;

protected:

private:
	virtual glm::dvec3 calculateDomainSize() override;

	double m_RadiusSlug;
	double m_RadiusOuter;
	double m_Thickness;
	glm::dvec3 m_Direction;
};

class VortexTubeVorticityDistribution : public VorticityDistribution
{
public:
	VortexTubeVorticityDistribution(const glm::dvec3 &center, double radius, double variation, double width, int periods, int location);
	virtual ~VortexTubeVorticityDistribution();

	virtual glm::dvec3 vorticityAtPosition(const glm::dvec3 &position) const override;

protected:

private:
	virtual glm::dvec3 calculateDomainSize() override;
	glm::dvec3 calculateCenter(const glm::dvec3 &center, int location, const double radius);

	double m_Radius;
	double m_Variation;
	double m_Width;
	double m_Wavenumber;
	int m_Location;
};

class NoiseVorticityDistribution : public VorticityDistribution
{
    public:
        NoiseVorticityDistribution(const glm::dvec3 &center, const glm::dvec3& extent);
        virtual ~NoiseVorticityDistribution();

        virtual glm::dvec3 vorticityAtPosition(const glm::dvec3 &position) const override;

    private:
        const glm::dvec3 m_Amplitude;
        const glm::dvec3 m_Extent;

        virtual glm::dvec3 calculateDomainSize() override;
};