
#include "Object.h"
#include "Scene.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

const glm::dvec3 Object::m_xAxis = glm::dvec3(1, 0, 0);
const glm::dvec3 Object::m_yAxis = glm::dvec3(0, 1, 0);
const glm::dvec3 Object::m_zAxis = glm::dvec3(0, 0, 1);
const glm::dvec3 Object::m_ForwardVector = -m_zAxis;
const glm::dvec3 Object::m_LeftVector = -m_xAxis;
const glm::dvec3 Object::m_BackwardVector = -m_ForwardVector;
const glm::dvec3 Object::m_RightVector = -m_LeftVector;

glm::dvec3 Camera::m_LocalLookDirection = Object::m_ForwardVector;
glm::dvec3 Camera::m_UpVector = Object::m_yAxis;

Object::Object()
{
	
}


Object::~Object()
{
}

Object * Object::copy() const
{
	return new Object(*this);
}

void Object::registerSceneHooks(Scene & scene)
{
}

void Object::deregisterSceneHooks(Scene & scene)
{
}

void Object::translate(const glm::dvec3 & delta)
{
	m_TranslationTransform *= glm::translate(glm::dmat4x4(1.0), delta);
}

void Object::rotateLocalX(double radians)
{
	rotate(radians, m_xAxis);
}

void Object::rotateLocalY(double radians)
{
	rotate(radians, m_yAxis);
}

void Object::scale(const glm::dvec3 & scaleVector)
{
	m_ScaleTransform = glm::scale(m_ScaleTransform, scaleVector);
}

glm::dvec3 Object::position() const
{
	return glm::dvec3(m_TranslationTransform[3]);
}

void Object::position(const glm::dvec3 & position)
{
	m_TranslationTransform[3] = glm::dvec4(position, 1.0);
}


void Object::rotate(double radians, const glm::dvec3 & axis)
{
	m_RotationTransform *= glm::rotate(glm::dmat4x4(1.0), radians, axis);
}