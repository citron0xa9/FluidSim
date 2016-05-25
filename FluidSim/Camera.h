#pragma once

#include "MovableObject.h"
#include <glm/trigonometric.hpp>

class Camera : public MovableObject
{
public:
	Camera(Scene &scene, float fovy = glm::radians(35.0f), float aspectRatio = 4.0f/3.0f, float near = 0.1f, float far = 100.0f);
	virtual ~Camera();

	virtual Object* copy() const override;

	glm::mat4x4 viewPerspectiveTransform() const;
	void aspectRatio(float ratio);
	void fovY(float f);

	virtual void translate(const glm::dvec3 &delta) override;

	glm::dvec3 lookDirection() const;

protected:
	virtual void rotate(double radians, const glm::dvec3 &axis) override;

private:
	void calculatePerspectiveTransform();
	void calculateViewTransform();
	void calculateViewPerspectiveTransform();

	static glm::dvec3 m_LocalLookDirection;
	static glm::dvec3 m_UpVector;

	float m_FovY;
	float m_AspectRatio;

	float m_NearClippingPlane;
	float m_FarClippingPlane;

	glm::mat4x4 m_ViewTransform;
	glm::mat4x4 m_PerspectiveTransform;
	glm::mat4x4 m_ViewPerspectiveTransform;
};

