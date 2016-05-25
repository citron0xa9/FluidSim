
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

void ContainerObject::render(const glm::mat4x4 &viewProjectTransform)
{
	m_UsingObjectListsRender.lock();
	for (auto object : m_DrawableObjectPtrs) {
		object->render(viewProjectTransform);
	}
	m_UsingObjectListsRender.unlock();
}

void ContainerObject::step(double secondsPassed)
{
	m_UsingObjectListsStep.lock();
	for (auto object : m_ActiveObjectPtrs) {
		object->step(secondsPassed);
	}
	m_UsingObjectListsStep.unlock();
}

void ContainerObject::addObject(const Object & object)
{
	m_UsingObjectListsRender.lock();
	m_UsingObjectListsStep.lock();

	m_ObjectPtrs.push_back(object.copy());
	m_ObjectPtrs.back()->registerContainerObjectHooks();

	m_UsingObjectListsStep.unlock();
	m_UsingObjectListsRender.unlock();
}

void ContainerObject::addObjectPtr(Object * objPtr)
{
	m_UsingObjectListsRender.lock();
	m_UsingObjectListsStep.lock();

	m_ObjectPtrs.push_back(objPtr);
	m_ObjectPtrs.back()->registerContainerObjectHooks();
	
	m_UsingObjectListsStep.unlock();
	m_UsingObjectListsRender.unlock();
}

void ContainerObject::removeObjectPtr(Object *objectPtr)
{
	m_UsingObjectListsRender.lock();
	m_UsingObjectListsStep.lock();

	objectPtr->deregisterContainerObjectHooks();
	m_ObjectPtrs.remove(objectPtr);
	
	m_UsingObjectListsStep.unlock();
	m_UsingObjectListsRender.unlock();
}

void ContainerObject::addActiveObject(ActiveObject & object)
{
	m_ActiveObjectPtrs.push_back(&object);
}

void ContainerObject::removeActiveObject(ActiveObject & object)
{
	m_ActiveObjectPtrs.remove(&object);
}

void ContainerObject::addDrawableObject(DrawableObject & object)
{
	m_DrawableObjectPtrs.push_back(&object);
}

void ContainerObject::removeDrawableObject(DrawableObject & object)
{
	m_DrawableObjectPtrs.remove(&object);
}
