#pragma once

#include <list>
#include <glm/mat4x4.hpp>
#include <mutex>

class Object;
class ActiveObject;
class DrawableObject;

class ContainerObject
{
public:
	ContainerObject();
	virtual ~ContainerObject();

	virtual void render(const glm::mat4x4 &viewProjectTransform);
	virtual void step(float secondsPassed);

	void addObject(const Object &object);
	void addObjectPtr(Object *objPtr);
	void removeObjectPtr(Object *objectPtr);

	void addActiveObject(ActiveObject &object);
	void removeActiveObject(ActiveObject &object);

	void addDrawableObject(DrawableObject &object);
	void removeDrawableObject(DrawableObject &object);

protected:
	std::list<Object*> m_ObjectPtrs;
	std::list<ActiveObject*> m_ActiveObjectPtrs;
	std::list<DrawableObject*> m_DrawableObjectPtrs;

private:
	std::mutex m_UsingObjectListsStep;
	std::mutex m_UsingObjectListsRender;
};

