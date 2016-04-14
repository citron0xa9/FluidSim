
#include "ContainerObject.h"


ContainerObject::ContainerObject()
{
}


ContainerObject::~ContainerObject()
{
	for (auto &elem : m_ObjectRefs) {
		delete &elem;
	}
}

void ContainerObject::render(const glm::mat4x4 &viewProjectTransform)
{
	for (auto object : m_DrawableObjectRefs) {
		object.render(viewProjectTransform);
	}
}

void ContainerObject::step(float secondsPassed)
{
	for (auto object : m_ActiveObjectRefs) {
		object.step(secondsPassed);
	}
}

void ContainerObject::addObject(const Object & object)
{
	m_ObjectRefs.push_back(*object.copy());
	m_ObjectRefs.back().registerContainerObjectHooks();
}

void ContainerObject::addActiveObject(ActiveObject & object)
{
	m_ActiveObjectRefs.push_back(object);
}

void ContainerObject::addDrawableObject(DrawableObject & object)
{
	m_DrawableObjectRefs.push_back(object);
}
