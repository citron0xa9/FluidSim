#include "Camera.h"
#include <iostream>
#include <glm/gtx/transform.hpp>


Camera::Camera(Scene &scene, float fovy, float aspectRatio, float near, float far) :
	Object{ scene },
	m_FovY{ fovy }, m_AspectRatio{ aspectRatio }, m_NearClippingPlane{ near }, m_FarClippingPlane{ far }
{
	calculateViewPerspectiveTransform();
}


Camera::~Camera()
{
}

Object * Camera::copy() const
{
	return new Camera{*this};
}

void Camera::calculateViewPerspectiveTransform()
{
	calculatePerspectiveTransform();
	calculateViewTransform();
	m_ViewPerspectiveTransform = m_PerspectiveTransform * m_ViewTransform;
}

inline glm::vec3 Camera::localLookDirection()
{
	return forwardVector();
}

inline glm::vec3 Camera::upVector()
{
	return yAxis();
}

void Camera::calculatePerspectiveTransform()
{
	m_PerspectiveTransform = glm::perspective(m_FovY, m_AspectRatio, m_NearClippingPlane, m_FarClippingPlane);
}

std::ostream& operator<<(std::ostream& os, const glm::vec3 &vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}

void Camera::calculateViewTransform()
{
	glm::vec3 position = glm::vec3(m_TranslationTransform*glm::vec4(origin(), 1.0));
	glm::vec3 lookAt = position + glm::vec3(m_RotationTransform*glm::vec4(localLookDirection(), 1.0));
	std::cout << "Calculating view tranform, with pos: " << position << "and look at " << lookAt << std::endl;
	m_ViewTransform = glm::lookAt(position, lookAt, upVector());
}

void Camera::setAspectRatio(float ratio)
{
	m_AspectRatio = ratio;
	calculateViewPerspectiveTransform();
}

glm::mat4x4 Camera::getViewPerspectiveTransform() const
{
	return m_ViewPerspectiveTransform;
}

void Camera::setFovY(float f) {
	m_FovY = f;
	calculateViewPerspectiveTransform();
}

void Camera::render(const glm::mat4x4 & viewProjectTransform) const
{
}

void Camera::translate(const glm::vec3 & delta)
{
	Object::translate(delta);
	calculateViewPerspectiveTransform();
}

glm::vec3 Camera::getLookDirection() const
{
	return glm::vec3(m_RotationTransform*glm::vec4(localLookDirection(),1.0));
}

void Camera::rotate(float degrees, const glm::vec3 & axis)
{
	Object::rotate(degrees, axis);
	std::cout << "Rotating " << degrees << " around (" << axis.x << ", " << axis.y << ", " << axis.z << ")" << std::endl;
	calculateViewPerspectiveTransform();
}
