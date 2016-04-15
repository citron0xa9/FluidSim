
#include "ContainerObject.h"
#include "Object.h"
#include "ActiveObject.h"
#include "DrawableObject.h"

ContainerObject::ContainerObject()
{
}


ContainerObject::~ContainerObject()
{
	for (auto elem : m_ObjectPtrs) {
		delete elem;
	}
}

void ContainerObject::render(const glm::mat4x4 &viewProjectTransform) const
{
	for (auto object : m_DrawableObjectPtrs) {
		object->render(viewProjectTransform);
	}
}

void ContainerObject::step(float secondsPassed)
{
	for (auto object : m_ActiveObjectPtrs) {
		object->step(secondsPassed);
	}
}

void ContainerObject::addObject(const Object & object)
{
	m_ObjectPtrs.push_back(object.copy());
	m_ObjectPtrs.back()->registerContainerObjectHooks();
}

void ContainerObject::addObjectPtr(Object * objPtr)
{
	m_ObjectPtrs.push_back(objPtr);
	m_ObjectPtrs.back()->registerContainerObjectHooks();
}

void ContainerObject::addActiveObject(ActiveObject & object)
{
	m_ActiveObjectPtrs.push_back(&object);
}

void ContainerObject::addDrawableObject(DrawableObject & object)
{
	m_DrawableObjectPtrs.push_back(&object);
}
