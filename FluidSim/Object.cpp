#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>

Object::Object(Scene & scene) : m_Scene{scene}
{
}

Object::~Object()
{
}

void Object::translate(const glm::vec3 & delta)
{
	m_TranslationTransform *= glm::translate(glm::mat4x4(1.0), delta);
}

void Object::rotateLocalX(float degrees)
{
	rotate(degrees, xAxis());
}

void Object::rotateLocalY(float degrees)
{
	rotate(degrees, yAxis());
}

void Object::addForwardVelocity(float velocity)
{
	addLocalVelocity(forwardVector()*velocity);
}

void Object::addBackwardVelocity(float velocity)
{
	addLocalVelocity(backwardVector()*velocity);
}

void Object::addLeftVelocity(float velocity)
{
	addLocalVelocity(leftVector()*velocity);
}

void Object::addRightVelocity(float velocity)
{
	addLocalVelocity(rightVector()*velocity);
}

void Object::step(float secondsPassed)
{
	updatePosition(secondsPassed);
}

glm::vec3 Object::xAxis()
{
	return glm::vec3(1, 0, 0);
}

glm::vec3 Object::yAxis()
{
	return glm::vec3(0, 1, 0);
}

glm::vec3 Object::zAxis()
{
	return glm::vec3(0, 0, 1);
}

inline glm::vec3 Object::forwardVector()
{
	return -zAxis();
}

inline glm::vec3 Object::leftVector()
{
	return -xAxis();
}

inline glm::vec3 Object::backwardVector()
{
	return -forwardVector();
}

inline glm::vec3 Object::rightVector()
{
	return -leftVector();
}

glm::vec3 Object::origin()
{
	return glm::vec3(0, 0, 0);
}

void Object::rotate(float degrees, const glm::vec3 & axis)
{
	m_RotationTransform *= glm::rotate(glm::mat4x4(1.0), degrees, axis);
}

void Object::addLocalVelocity(const glm::vec3 & velocity)
{
	m_LocalVelocity += velocity;
}

void Object::updatePosition(float secondsPassed)
{
	glm::vec3 globalVelocity = glm::vec3(m_RotationTransform*glm::vec4(m_LocalVelocity, 1.0));
	glm::vec3 translationDelta = globalVelocity*secondsPassed;
	translate(translationDelta);
}
