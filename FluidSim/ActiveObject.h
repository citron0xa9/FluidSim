#pragma once

#include "ContainerObject.h"

class ActiveObject : public Object
{
public:
	ActiveObject(ContainerObject &container);
	virtual ~ActiveObject();

	virtual void step(float secondsPassed) = 0;

	virtual void registerContainerObjectHooks() override;
};

