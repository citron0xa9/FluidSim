#pragma once

#include "TriangleNetObject.h"

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <cmath>

const double DEFAULT_VORTON_RADIUS = FLT_EPSILON * 100;
const double SIGNIFICANT_VORTICITY = expf(0.5f * (logf(FLT_EPSILON) + logf(FLT_MIN)));
const double ONE_OVER_FOUR_PI = 0.25f * glm::one_over_pi<double>();

class Vorton : public TriangleNetObject
{
public:
	Vorton(const TriangleNetObject &triangleNetObj);
	Vorton(const TriangleNetObject &triangleNetObj, 
		const glm::dvec3 &position, const glm::dvec3 &vorticity, double radius = DEFAULT_VORTON_RADIUS);

	virtual ~Vorton();

	glm::dvec3 vorticity() const;
	void vorticity(const glm::dvec3 &vorticity);
	double radius() const;
	void radius(double radius);

	virtual Object* copy() const override;
	glm::dvec3 inducedVelocity(const glm::dvec3 &position) const;

private:
	glm::dvec3 m_Vorticity;
	double m_Radius;
	glm::dvec3 m_Velocity;
};

