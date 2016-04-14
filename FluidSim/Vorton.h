#pragma once

#include "TriangleNetObject.h"

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <cmath>

const float DEFAULT_VORTON_RADIUS = FLT_EPSILON * 100;
const float SIGNIFICANT_VORTICITY = expf(0.5 * (logf(FLT_EPSILON) + logf(FLT_MIN)));

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

	void setVorticity(const glm::vec3 &vorticity);

	void setIsRendered(bool isRendered);
	virtual void render(const glm::mat4x4 &viewProjectTransform) const override;

protected:
	
private:
	glm::vec3 m_Position;
	glm::vec3 m_Vorticity;
	float m_Radius;
	glm::vec3 m_Velocity;

	bool m_IsRendered;
};

