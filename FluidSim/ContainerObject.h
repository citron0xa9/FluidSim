#pragma once

#include <list>
#include <glm/mat4x4.hpp>

class Object;
class ActiveObject;
class DrawableObject;

class ContainerObject
{
public:
	ContainerObject();
	virtual ~ContainerObject();

	virtual void render(const glm::mat4x4 &viewProjectTransform) const;
	virtual void step(float secondsPassed);

	void addObject(const Object &object);
	void addObjectPtr(Object *objPtr);
	void addActiveObject(ActiveObject &object);
	void addDrawableObject(DrawableObject &object);

protected:
	std::list<Object*> m_ObjectPtrs;
	std::list<ActiveObject*> m_ActiveObjectPtrs;
	std::list<DrawableObject*> m_DrawableObjectPtrs;
};

