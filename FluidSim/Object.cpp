
#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>

const glm::vec3 Object::m_xAxis = glm::vec3(1, 0, 0);
const glm::vec3 Object::m_yAxis = glm::vec3(0, 1, 0);
const glm::vec3 Object::m_zAxis = glm::vec3(0, 0, 1);
const glm::vec3 Object::m_ForwardVector = -m_zAxis;
const glm::vec3 Object::m_LeftVector = -m_xAxis;
const glm::vec3 Object::m_BackwardVector = -m_ForwardVector;
const glm::vec3 Object::m_RightVector = -m_LeftVector;

Object::Object(ContainerObject &container) : m_ContainerObj{container}
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
	rotate(degrees, m_xAxis);
}

void Object::rotateLocalY(float degrees)
{
	rotate(degrees, m_yAxis);
}

glm::vec3 Object::getPosition() const
{
	return glm::vec3(m_TranslationTransform[3]);
}

void Object::setPosition(const glm::vec3 & position)
{
	m_TranslationTransform[3] = glm::vec4(position, 1.0);
}

void Object::rotate(float degrees, const glm::vec3 & axis)
{
	m_RotationTransform *= glm::rotate(glm::mat4x4(1.0), degrees, axis);
}