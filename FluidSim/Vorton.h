#pragma once

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <cmath>

class Vorton
{
public:
	Vorton();
	Vorton(const glm::vec3 &position, const glm::vec3 &vorticity, float radius);
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

