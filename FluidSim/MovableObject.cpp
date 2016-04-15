#include "MovableObject.h"


MovableObject::MovableObject(ContainerObject & container) : ActiveObject{container}, Object{container}
{
}

MovableObject::~MovableObject()
{
}

void MovableObject::addForwardVelocity(float velocity)
{
	addLocalVelocity(m_ForwardVector*velocity);
}

void MovableObject::addBackwardVelocity(float velocity)
{
	addLocalVelocity(m_BackwardVector*velocity);
}

void MovableObject::addLeftVelocity(float velocity)
{
	addLocalVelocity(m_LeftVector*velocity);
}

void MovableObject::addRightVelocity(float velocity)
{
	addLocalVelocity(m_RightVector*velocity);
}

void MovableObject::step(float secondsPassed)
{
	updatePosition(secondsPassed);
}



void MovableObject::addLocalVelocity(const glm::vec3 & velocity)
{
	m_LocalVelocity += velocity;
}

void MovableObject::updatePosition(float secondsPassed)
{
	glm::vec3 globalVelocity = glm::vec3(m_RotationTransform*glm::vec4(m_LocalVelocity, 1.0));
	glm::vec3 translationDelta = globalVelocity*secondsPassed;
	translate(translationDelta);
}
