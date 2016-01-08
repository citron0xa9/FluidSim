#include "Camera.h"
#include <glm/gtx/transform.hpp>


Camera::Camera(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up, float fovy, float aspectRatio, float near, float far) :
	m_Position{ position }, m_LookAt{ lookAt }, m_Up{ up }, m_FovY{ fovy }, m_AspectRatio{ aspectRatio }, m_NearClippingPlane{ near }, m_FarClippingPlane{ far }
{
	calculateViewPerspectiveTransform();
}


Camera::~Camera()
{
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
	m_ViewTransform = glm::lookAt(m_Position, m_LookAt, m_Up);
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

glm::vec3 Camera::getLookAt() const
{
	return m_LookAt;
}
