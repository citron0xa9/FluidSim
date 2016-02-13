#pragma once

#include "Object.h"
#include <glm/trigonometric.hpp>

class Camera : public Object
{
public:
	Camera(Scene &scene, float fovy = glm::radians(35.0f), float aspectRatio = 4.0f/3.0f, float near = 0.1f, float far = 100.0f);
	virtual ~Camera();

	virtual Object* copy() const override;

	glm::mat4x4 getViewPerspectiveTransform() const;
	void setAspectRatio(float ratio);
	void setFovY(float f);

	virtual void render(const glm::mat4x4 &viewProjectTransform) const override;

	virtual void translate(const glm::vec3 &delta) override;

	glm::vec3 getLookDirection() const;
	glm::vec3 getPosition() const;

protected:
	virtual void rotate(float degrees, const glm::vec3 &axis) override;

private:
	void calculatePerspectiveTransform();
	void calculateViewTransform();
	void calculateViewPerspectiveTransform();

	static glm::vec3 localLookDirection();
	static glm::vec3 upVector();

	float m_FovY;
	float m_AspectRatio;

	float m_NearClippingPlane;
	float m_FarClippingPlane;

	glm::mat4x4 m_ViewTransform;
	glm::mat4x4 m_PerspectiveTransform;
	glm::mat4x4 m_ViewPerspectiveTransform;
};

