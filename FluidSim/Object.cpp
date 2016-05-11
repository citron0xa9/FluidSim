
#include "Object.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

const glm::vec3 Object::m_xAxis = glm::vec3(1, 0, 0);
const glm::vec3 Object::m_yAxis = glm::vec3(0, 1, 0);
const glm::vec3 Object::m_zAxis = glm::vec3(0, 0, 1);
const glm::vec3 Object::m_ForwardVector = -m_zAxis;
const glm::vec3 Object::m_LeftVector = -m_xAxis;
const glm::vec3 Object::m_BackwardVector = -m_ForwardVector;
const glm::vec3 Object::m_RightVector = -m_LeftVector;

glm::vec3 Camera::m_LocalLookDirection = Object::m_ForwardVector;
glm::vec3 Camera::m_UpVector = Object::m_yAxis;

Object::Object(ContainerObject &container) : m_ContainerObjectPtr{&container}
{
	
}


Object::~Object()
{
}

Object * Object::copy() const
{
	return new Object(*this);
}

void Object::registerContainerObjectHooks()
{
	
}

void Object::deregisterContainerObjectHooks()
{
}

void Object::translate(const glm::vec3 & delta)
{
	m_TranslationTransform *= glm::translate(glm::mat4x4(1.0), delta);
}

void Object::rotateLocalX(float radians)
{
	rotate(radians, m_xAxis);
}

void Object::rotateLocalY(float radians)
{
	rotate(radians, m_yAxis);
}

void Object::scale(const glm::vec3 & scaleVector)
{
	m_ScaleTransform = glm::scale(m_ScaleTransform, scaleVector);
}

glm::vec3 Object::position() const
{
	return glm::vec3(m_TranslationTransform[3]);
}

void Object::position(const glm::vec3 & position)
{
	m_TranslationTransform[3] = glm::vec4(position, 1.0);
}

void Object::containedIn(ContainerObject & containerObject)
{
	m_ContainerObjectPtr = &containerObject;
}

void Object::rotate(float radians, const glm::vec3 & axis)
{
	m_RotationTransform *= glm::rotate(glm::mat4x4(1.0), radians, axis);
}