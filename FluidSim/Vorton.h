#pragma once

#include "TriangleNetObject.h"

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <cmath>

const float DEFAULT_VORTON_RADIUS = FLT_EPSILON * 100;
const float SIGNIFICANT_VORTICITY = expf(0.5f * (logf(FLT_EPSILON) + logf(FLT_MIN)));
const float ONE_OVER_FOUR_PI = 0.25f * glm::one_over_pi<float>();

class Vorton : public TriangleNetObject
{
public:
	Vorton(const TriangleNetObject &triangleNetObj);
	Vorton(const TriangleNetObject &triangleNetObj, 
		const glm::vec3 &position, const glm::vec3 &vorticity, float radius = DEFAULT_VORTON_RADIUS);

	virtual ~Vorton();

	glm::vec3 inducedVelocity(const glm::vec3 &position) const;
	glm::vec3 getVorticity() const;
	float getRadius() const;
	void setRadius(float radius);

	void setVorticity(const glm::vec3 &vorticity);

	virtual Object* copy() const override;


protected:
	
private:

	glm::vec3 m_Vorticity;
	float m_Radius;
	glm::vec3 m_Velocity;
};

