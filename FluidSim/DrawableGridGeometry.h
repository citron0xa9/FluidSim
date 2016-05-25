#pragma once

#include "DrawableObject.h"
#include "UniformGridGeometry.h"

class DrawableGridGeometry : public UniformGridGeometry, public DrawableObject
{
public:
	DrawableGridGeometry(const UniformGridGeometry &geometry, ContainerObject &container);
	virtual ~DrawableGridGeometry();
};

