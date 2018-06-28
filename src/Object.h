#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <mutex>
#include <list>

class Scene;

class Object
{
public:
	Object();
	Object(const Object &object);
	virtual ~Object();

	virtual Object* copy() const;
	virtual void registerSceneHooks(Scene &scene);
	virtual void deregisterSceneHooks(Scene &scene);

	//void addToScene(Scene &scene);
	//void removeFromScene();

	virtual void translate(const glm::dvec3 &delta);
	void rotateLocalX(double radians);
	void rotateLocalY(double radians);
	virtual void rotate(double radians, const glm::dvec3 &axis);
	virtual void resetRotation();

	void scale(const glm::dvec3 &scaleVector);

	glm::dvec3 position() const;
	void position(const glm::dvec3 &position);

	unsigned int id() const;

	virtual void printInfo() const;

	static Object& objectWithId(unsigned int id);

protected:
	static const glm::dvec3 m_xAxis;
	static const glm::dvec3 m_yAxis;
	static const glm::dvec3 m_zAxis;

	static const glm::dvec3 m_ForwardVector;
	static const glm::dvec3 m_LeftVector;
	static const glm::dvec3 m_BackwardVector;
	static const glm::dvec3 m_RightVector;

	glm::dmat4x4 m_RotationTransform;
	glm::dmat4x4 m_TranslationTransform;
	glm::dmat4x4 m_ScaleTransform;

	//Scene *m_ScenePtr;

private:
	void getId();

	unsigned int m_Id;
	static std::vector<Object*> m_AllObjects;
	static std::list<unsigned int> m_ReusableIds;
	static std::mutex m_ReusableIdsLock;
	static std::mutex m_AllObjectsLock;
};

