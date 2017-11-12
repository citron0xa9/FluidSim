#pragma once

#include "Tracer.h"

#include "Log.h"
#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <cmath>

const double DEFAULT_VORTON_RADIUS = FLT_EPSILON * 100;
const double SIGNIFICANT_VORTICITY = expf(0.5f * (logf(FLT_EPSILON) + logf(FLT_MIN)));

class Vorton : public Tracer
{
public:
	Vorton();
	Vorton(const glm::dvec3 &position, const glm::dvec3 &vorticity, double radius = DEFAULT_VORTON_RADIUS, const double mass = 0.0);

	virtual ~Vorton();

	glm::dvec3 vorticity() const;
	void vorticity(const glm::dvec3 &vorticity);
    void vorticityByVelocity(const glm::dvec3& velocity, const glm::dvec3& positionOfVelocity);

	virtual Object* copy() const override;
	glm::dvec3 inducedVelocity(const glm::dvec3 &position) const;

	virtual void printInfo() const override;

private:
    static const double ONE_OVER_4_PI;
    static const double VOLUME_CONSTANT;
    static const double VELOCITY_FROM_VORTICITY_CONSTANT;
    static const double VORTICITY_FROM_VELOCITY_CONSTANT;

	glm::dvec3 m_Vorticity;
};

