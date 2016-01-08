#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, -5.0f), glm::vec3 lookAt = glm::vec3(0.0,0.0,0.0), glm::vec3 up = glm::vec3(0.0,1.0,0.0), float fovy = 35.0f, float aspectRatio = 4.0f/3.0f, float near = 0.1f, float far = 100.0f);
	virtual ~Camera();

	glm::mat4x4 getViewPerspectiveTransform() const;
	glm::vec3 getLookAt() const;
	void setAspectRatio(float ratio);

private:
	void calculatePerspectiveTransform();
	void calculateViewTransform();
	void calculateViewPerspectiveTransform();

	glm::vec3 m_Position;
	glm::vec3 m_LookAt;
	glm::vec3 m_Up;

	float m_FovY;
	float m_AspectRatio;

	float m_NearClippingPlane;
	float m_FarClippingPlane;

	glm::mat4x4 m_ViewTransform;
	glm::mat4x4 m_PerspectiveTransform;
	glm::mat4x4 m_ViewPerspectiveTransform;
};

