#include "LightSource.h"

#include "Scene.h"


LightSource::LightSource(Scene &scene, GLfloat intensity) : Object{ scene }, m_Intensity { intensity }
{
	
}


LightSource::~LightSource()
{
}

void LightSource::registerContainerObjectHooks()
{
	registerSceneHooks(static_cast<Scene&>(*m_ContainerObjPtr));
}

void LightSource::registerSceneHooks(Scene & scene)
{
	scene.addLightSource(*this);
}
