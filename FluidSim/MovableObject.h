#pragma once

#include "ActiveObject.h"

class Scene;

class MovableObject : public ActiveObject
{
public:
	MovableObject(ContainerObject &container);
	virtual ~MovableObject();

	void addForwardVelocity(float velocity);
	void addBackwardVelocity(float velocity);
	void addLeftVelocity(float velocity);
	void addRightVelocity(float velocity);

	virtual void step(float secondsPassed) override;

private:

	void addLocalVelocity(const glm::vec3 &velocity);
	void updatePosition(float secondsPassed);
	
	glm::vec3 m_LocalVelocity;
};

