#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Scene;

class Object
{
public:
	Object();
	virtual ~Object();

	virtual Object* copy() const;
	virtual void registerSceneHooks(Scene &scene);
	virtual void deregisterSceneHooks(Scene &scene);

	virtual void translate(const glm::dvec3 &delta);
	void rotateLocalX(double radians);
	void rotateLocalY(double radians);
	void scale(const glm::dvec3 &scaleVector);

	glm::dvec3 position() const;
	void position(const glm::dvec3 &position);

protected:
	static const glm::dvec3 m_xAxis;
	static const glm::dvec3 m_yAxis;
	static const glm::dvec3 m_zAxis;

	static const glm::dvec3 m_ForwardVector;
	static const glm::dvec3 m_LeftVector;
	static const glm::dvec3 m_BackwardVector;
	static const glm::dvec3 m_RightVector;


	virtual void rotate(double radians, const glm::dvec3 &axis);

	glm::dmat4x4 m_RotationTransform;
	glm::dmat4x4 m_TranslationTransform;
	glm::dmat4x4 m_ScaleTransform;
};

