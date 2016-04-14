#include "Camera.h"
#include <iostream>
#include <glm/gtx/transform.hpp>


Camera::Camera(Scene &scene, float fovy, float aspectRatio, float near, float far) :
	MovableObject{ scene },
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
	return m_ForwardVector;
}

inline glm::vec3 Camera::upVector()
{
	return m_yAxis;
}

void Camera::calculatePerspectiveTransform()
{
	m_PerspectiveTransform = glm::perspective(m_FovY, m_AspectRatio, m_NearClippingPlane, m_FarClippingPlane);
}

void Camera::calculateViewTransform()
{
	glm::vec3 position = getPosition();
	glm::vec3 lookAt = position + glm::vec3(m_RotationTransform*glm::vec4(localLookDirection(), 1.0));
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
	if (delta == glm::vec3(0)) {
		return;
	}
	Object::translate(delta);
	calculateViewPerspectiveTransform();
}

glm::vec3 Camera::getLookDirection() const
{
	return glm::vec3(m_RotationTransform*glm::vec4(localLookDirection(),1.0));
}

void Camera::rotate(float degrees, const glm::vec3 & axis)
{
	if (degrees == 0) {
		return;
	}
	Object::rotate(degrees, axis);
	calculateViewPerspectiveTransform();
}
