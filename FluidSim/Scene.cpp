#include "Scene.h"


Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::addObject(const Object &obj)
{
	m_Objects.push_back(obj);
}

void Scene::addLightSource(const LightSource &lightSource)
{
	m_LightSources.push_back(lightSource);
}

void Scene::render()
{
	//call render on each object
	std::for_each(m_Objects.begin(), m_Objects.end(), [](const Object &obj){obj.render(); });
}
