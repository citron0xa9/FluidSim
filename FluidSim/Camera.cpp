#include "Camera.h"
#include "Scene.h"
#include <iostream>
#include <glm/gtx/transform.hpp>


Camera::Camera(Scene &scene, float fovy, float aspectRatio, float near, float far) :
	MovableObject{ scene }, Object{ scene },
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

void Camera::calculatePerspectiveTransform()
{
	m_PerspectiveTransform = glm::perspective(m_FovY, m_AspectRatio, m_NearClippingPlane, m_FarClippingPlane);
}

void Camera::calculateViewTransform()
{
	glm::vec3 currentPosition = position();
	glm::vec3 lookAt = currentPosition + glm::vec3(m_RotationTransform*glm::vec4(m_LocalLookDirection, 1.0));
	m_ViewTransform = glm::lookAt(currentPosition, lookAt, m_UpVector);
}

void Camera::aspectRatio(float ratio)
{
	m_AspectRatio = ratio;
	calculateViewPerspectiveTransform();
}

glm::mat4x4 Camera::viewPerspectiveTransform() const
{
	return m_ViewPerspectiveTransform;
}

void Camera::fovY(float f) {
	m_FovY = f;
	calculateViewPerspectiveTransform();
}

void Camera::translate(const glm::vec3 & delta)
{
	if (delta == glm::vec3(0)) {
		return;
	}
	Object::translate(delta);
	calculateViewPerspectiveTransform();
}

glm::vec3 Camera::lookDirection() const
{
	return glm::vec3(m_RotationTransform*glm::vec4(m_LocalLookDirection,1.0));
}

void Camera::rotate(float radians, const glm::vec3 & axis)
{
	if (radians == 0) {
		return;
	}
	Object::rotate(radians, axis);
	calculateViewPerspectiveTransform();
}
