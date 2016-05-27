#include "LightSource.h"

#include "Scene.h"


LightSource::LightSource(Scene &scene, GLfloat intensity) : m_Intensity { intensity }
{
	
}


LightSource::~LightSource()
{
}

void LightSource::registerSceneHooks(Scene & scene)
{
	scene.addLightSource(*this);
}
