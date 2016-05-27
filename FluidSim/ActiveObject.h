#pragma once

#include "Object.h"

class ActiveObject : public virtual Object
{
public:
	ActiveObject();
	virtual ~ActiveObject();

	virtual void step(double secondsPassed) = 0;

	virtual void registerSceneHooks(Scene &scene) override;
	virtual void deregisterSceneHooks(Scene &scene) override;
};

