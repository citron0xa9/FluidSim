#pragma once

#include "ActiveObject.h"

class Scene;

class MovableObject : public ActiveObject
{
public:
	MovableObject(ContainerObject &container);
	virtual ~MovableObject();

	void startMoveForward();
	void startMoveLeft();
	void startMoveRight();
	void startMoveBack();

	void stopMoveForward();
	void stopMoveLeft();
	void stopMoveRight();
	void stopMoveBack();

	virtual void step(float secondsPassed) override;

private:

	void addLocalVelocity(const glm::vec3 &velocity);
	void updatePosition(float secondsPassed);

	float m_MovementVelocityFactor;
	
	glm::vec3 m_LocalVelocity;
};

