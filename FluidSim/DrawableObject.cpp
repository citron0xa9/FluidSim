
#include "DrawableObject.h"
#include "Scene.h"

DrawableObject::DrawableObject()
{
}


DrawableObject::~DrawableObject()
{
}

void DrawableObject::registerSceneHooks(Scene &scene)
{
	scene.addDrawableObject(*this);
}

void DrawableObject::deregisterSceneHooks(Scene &scene)
{
	scene.removeDrawableObject(*this);
}
