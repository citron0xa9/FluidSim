#pragma once

#include "../Object.h"

class Arrow : public Object
{
public:
	Arrow(const glm::dvec3 &velocity, float angle);
	virtual ~Arrow();

	virtual Object* copy() const override;
	virtual void printInfo() const override;

	void velocity(const glm::dvec3 &velocity);
	void velocityNormalized(const glm::dvec3 &velocity);
	void angle(float angle);

	void toggleHighlight();
	bool isHighlighted() const;

private:
	glm::dvec3 m_Velocity;
	glm::dvec3 m_VelocityNormalized;
	float m_Angle;

	bool m_IsHighlighted;
};

