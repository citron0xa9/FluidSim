#pragma once

#include "ActiveObject.h"

class Scene;

class MovableObject : public ActiveObject
{
public:
	MovableObject();
	virtual ~MovableObject();

	void startMoveForward();
	void startMoveLeft();
	void startMoveRight();
	void startMoveBack();

	void stopMoveForward();
	void stopMoveLeft();
	void stopMoveRight();
	void stopMoveBack();

	virtual void step(double secondsPassed) override;

private:

	void addLocalVelocity(const glm::dvec3 &velocity);
	void updatePosition(double secondsPassed);

	double m_MovementVelocityFactor;
	
	glm::dvec3 m_LocalVelocity;
};

