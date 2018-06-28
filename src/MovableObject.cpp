#include "MovableObject.h"


MovableObject::MovableObject() : m_MovementVelocityFactor{4.0}
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

void MovableObject::step(double secondsPassed)
{
	updatePosition(secondsPassed);
}



void MovableObject::addLocalVelocity(const glm::dvec3 & velocity)
{
	m_LocalVelocity += velocity;
}

void MovableObject::updatePosition(double secondsPassed)
{
	glm::dvec3 globalVelocity = glm::dvec3(m_RotationTransform*glm::vec4(m_LocalVelocity, 1.0));
	glm::dvec3 translationDelta = globalVelocity*secondsPassed;
	translate(translationDelta);
}
