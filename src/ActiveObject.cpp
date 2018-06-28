
#include "ActiveObject.h"
#include "Scene.h"


ActiveObject::ActiveObject()
{
}


ActiveObject::~ActiveObject()
{
}

void ActiveObject::registerSceneHooks(Scene &scene)
{
	scene.addActiveObject(*this);
}

void ActiveObject::deregisterSceneHooks(Scene &scene)
{
	scene.removeActiveObject(*this);
}
