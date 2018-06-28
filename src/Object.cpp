
#include "Object.h"
#include "Scene.h"
#include "graphics/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

const glm::dvec3 Object::m_xAxis = glm::dvec3(1, 0, 0);
const glm::dvec3 Object::m_yAxis = glm::dvec3(0, 1, 0);
const glm::dvec3 Object::m_zAxis = glm::dvec3(0, 0, 1);
const glm::dvec3 Object::m_ForwardVector = -m_zAxis;
const glm::dvec3 Object::m_LeftVector = -m_xAxis;
const glm::dvec3 Object::m_BackwardVector = -m_ForwardVector;
const glm::dvec3 Object::m_RightVector = -m_LeftVector;

glm::dvec3 Camera::m_LocalLookDirection = Object::m_ForwardVector;
glm::dvec3 Camera::m_UpVector = Object::m_yAxis;

std::vector<Object*> Object::m_AllObjects{};
std::list<unsigned int> Object::m_ReusableIds{};
std::mutex Object::m_ReusableIdsLock{};
std::mutex Object::m_AllObjectsLock{};


Object::Object()
    : m_TranslationTransform{1}, m_ScaleTransform{1}, m_RotationTransform{1}
{
	getId();
}

Object::Object(const Object & object)
	: m_TranslationTransform{object.m_TranslationTransform}, m_RotationTransform{object.m_RotationTransform}, m_ScaleTransform{object.m_ScaleTransform}
{
	getId();
}


Object::~Object()
{
	m_AllObjects[m_Id] = nullptr;
	m_ReusableIdsLock.lock();
	m_ReusableIds.push_back(m_Id);
	m_ReusableIdsLock.unlock();
}

Object * Object::copy() const
{
	return new Object(*this);
}

void Object::registerSceneHooks(Scene & scene)
{
}

void Object::deregisterSceneHooks(Scene & scene)
{
}

/*void Object::addToScene(Scene & scene)
{
	if (m_ScenePtr != nullptr) {
		throw std::runtime_error("Object::addToScene: object can only be added to 0-1 scenes");
	}
	m_ScenePtr = &scene;
	registerSceneHooks(scene);
}

void Object::removeFromScene()
{
	if (m_ScenePtr == nullptr) {
		throw std::runtime_error("Object::removeFromScene: No scene to remove from");
	}
	deregisterSceneHooks(*m_ScenePtr);
}*/

void Object::translate(const glm::dvec3 & delta)
{
	m_TranslationTransform *= glm::translate(glm::dmat4x4(1.0), delta);
}

void Object::rotateLocalX(double radians)
{
	rotate(radians, m_xAxis);
}

void Object::rotateLocalY(double radians)
{
	rotate(radians, m_yAxis);
}

void Object::scale(const glm::dvec3 & scaleVector)
{
	m_ScaleTransform = glm::scale(m_ScaleTransform, scaleVector);
}

glm::dvec3 Object::position() const
{
	return glm::dvec3(m_TranslationTransform[3]);
}

void Object::position(const glm::dvec3 & position)
{
	m_TranslationTransform[3] = glm::dvec4(position, 1.0);
}

unsigned int Object::id() const
{
	return m_Id;
}

void Object::printInfo() const
{
}

Object& Object::objectWithId(unsigned int id)
{
	return *(m_AllObjects[id]);
}

void Object::getId()
{
	m_AllObjectsLock.lock();
	m_ReusableIdsLock.lock();
	if (!m_ReusableIds.empty()) {
		m_Id = m_ReusableIds.front();
		m_ReusableIds.pop_front();
		m_AllObjects[m_Id] = this;
	}
	else {
		m_AllObjects.push_back(this);
		m_Id = m_AllObjects.size() - 1;
	}
	m_ReusableIdsLock.unlock();
	m_AllObjectsLock.unlock();
}


void Object::rotate(double radians, const glm::dvec3 & axis)
{
	m_RotationTransform *= glm::rotate(glm::dmat4x4(1.0), radians, axis);
}

void Object::resetRotation()
{
	m_RotationTransform = glm::dmat4x4(1);
}
