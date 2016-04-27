#pragma once

#include "Object.h"

class DrawableObject : public virtual Object
{
public:
	DrawableObject(ContainerObject &container);
	virtual ~DrawableObject();

	virtual void render(const glm::mat4x4 &viewProjectTransform) const = 0;

	virtual void registerContainerObjectHooks() override;
};

