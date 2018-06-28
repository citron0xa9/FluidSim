#pragma once

#include "Particle.h"

#include "../util/Log.h"
#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <cmath>
#include <limits>

const double DEFAULT_VORTON_RADIUS = std::numeric_limits<float>::epsilon() * 100;
const double SIGNIFICANT_VORTICITY = expf(0.5f * (logf(std::numeric_limits<float>::epsilon()) + logf(std::numeric_limits<float>::min())));

class Vorton : public Particle
{
public:
	Vorton(const double birthTimeSeconds, double radius, const double mass);
	Vorton(const glm::dvec3 &position, const glm::dvec3 &vorticity, const double birthTimeSeconds, double radius, const double mass);

	virtual ~Vorton();

	glm::dvec3 vorticity() const;
	void vorticity(const glm::dvec3 &vorticity);
    void vorticityByVelocity(const glm::dvec3& velocity, const glm::dvec3& positionOfVelocity);

	virtual Vorton* copy() const override;
	glm::dvec3 inducedVelocity(const glm::dvec3 &position) const;

	virtual void printInfo() const override;

private:
    static const double ONE_OVER_4_PI;
    static const double VOLUME_CONSTANT;
    static const double VELOCITY_FROM_VORTICITY_CONSTANT;
    static const double VORTICITY_FROM_VELOCITY_CONSTANT;

	glm::dvec3 m_Vorticity;
};

