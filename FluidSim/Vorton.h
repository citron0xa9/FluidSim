#pragma once

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <cmath>

const float DEFAULT_VORTON_RADIUS = FLT_EPSILON * 100;
const float SIGNIFICANT_VORTICITY = expf(0.5 * (logf(FLT_EPSILON) + logf(FLT_MIN)));

class Vorton
{
public:
	Vorton();
	Vorton(const glm::vec3 &position, const glm::vec3 &vorticity, float radius = DEFAULT_VORTON_RADIUS);
	virtual ~Vorton();

	glm::vec3 inducedVelocity(const glm::vec3 &position) const;
	const glm::vec3 getVorticity() const;
	const glm::vec3 getPosition() const;
	float getRadius() const;
protected:
	void setVorticity(const glm::vec3 &vorticity);
	void setPosition(const glm::vec3 &position);
private:
	glm::vec3 m_Position;
	glm::vec3 m_Vorticity;
	float m_Radius;
	glm::vec3 m_Velocity;
};

