#include "MovableObject.h"


MovableObject::MovableObject(ContainerObject & container) : ActiveObject{container}, Object{container}, m_MovementVelocityFactor{4.0f}
{
}

MovableObject::~MovableObject()
{
}

void MovableObject::startMoveForward()
{
	m_LocalVelocity += m_MovementVelocityFactor * m_ForwardVector;
}

void MovableObject::startMoveLeft()
{
	m_LocalVelocity += m_MovementVelocityFactor * m_LeftVector;
}

void MovableObject::startMoveRight()
{
	m_LocalVelocity += m_MovementVelocityFactor * m_RightVector;
}

void MovableObject::startMoveBack()
{
	m_LocalVelocity += m_MovementVelocityFactor * m_BackwardVector;
}

void MovableObject::stopMoveForward()
{
	m_LocalVelocity -= m_MovementVelocityFactor * m_ForwardVector;
}

void MovableObject::stopMoveLeft()
{
	m_LocalVelocity -= m_MovementVelocityFactor * m_LeftVector;
}

void MovableObject::stopMoveRight()
{
	m_LocalVelocity -= m_MovementVelocityFactor * m_RightVector;
}

void MovableObject::stopMoveBack()
{
	m_LocalVelocity -= m_MovementVelocityFactor * m_BackwardVector;
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
