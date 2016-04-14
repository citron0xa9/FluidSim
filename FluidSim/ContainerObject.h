#pragma once

#include "Object.h"
#include "ActiveObject.h"
#include "DrawableObject.h"
#include <list>

class ContainerObject
{
public:
	ContainerObject();
	virtual ~ContainerObject();

	void render(const glm::mat4x4 &viewProjectTransform);
	void step(float secondsPassed);

	virtual void addObject(const Object &object);
	void addActiveObject(ActiveObject &object);
	void addDrawableObject(DrawableObject &object);

protected:
	std::list<Object&> m_ObjectRefs;

private:
	std::list<ActiveObject&> m_ActiveObjectRefs;
	std::list<DrawableObject&> m_DrawableObjectRefs;
};

