#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "ContainerObject.h"

class Object
{
public:
	Object(ContainerObject &container);
	virtual ~Object();

	virtual Object* copy() const = 0;
	virtual void registerContainerObjectHooks() = 0;

	virtual void translate(const glm::vec3 &delta);
	void rotateLocalX(float degrees);
	void rotateLocalY(float degrees);
	void scale(const glm::vec3 &scaleVector);

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3 &position);

	void setContainerObject(ContainerObject &containerObject);

	//ContainerObject& getContainerObject();
protected:
	static const glm::vec3 m_xAxis;
	static const glm::vec3 m_yAxis;
	static const glm::vec3 m_zAxis;

	static const glm::vec3 m_ForwardVector;
	static const glm::vec3 m_LeftVector;
	static const glm::vec3 m_BackwardVector;
	static const glm::vec3 m_RightVector;


	virtual void rotate(float degrees, const glm::vec3 &axis);

	glm::mat4x4 m_RotationTransform;
	glm::mat4x4 m_TranslationTransform;
	glm::mat4x4 m_ScaleTransform;

	ContainerObject *m_ContainerObjPtr;
private:
	
};

