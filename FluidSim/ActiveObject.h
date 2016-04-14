#pragma once

#include "Object.h"

class ActiveObject : public virtual Object
{
public:
	ActiveObject(ContainerObject &container);
	virtual ~ActiveObject();

	virtual void step(float secondsPassed) = 0;

	virtual void registerContainerObjectHooks() override;
};

