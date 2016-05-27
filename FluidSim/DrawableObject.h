#pragma once

#include "Object.h"

class DrawableObject : public virtual Object
{
public:
	DrawableObject();
	virtual ~DrawableObject();

	virtual void render(const glm::mat4x4 &viewProjectTransform) = 0;

	virtual void registerSceneHooks(Scene &scene) override;
	virtual void deregisterSceneHooks(Scene &scene) override;
};

