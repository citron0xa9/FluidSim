
#include "DrawableObject.h"


DrawableObject::DrawableObject(ContainerObject &container) : Object{container}
{
}


DrawableObject::~DrawableObject()
{
}

void DrawableObject::registerContainerObjectHooks()
{
	m_ContainerObj.addDrawableObject(*this);
}
