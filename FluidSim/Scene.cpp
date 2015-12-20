#include "Scene.h"


Scene::Scene()
{
}


Scene::~Scene()
{
	for (auto geometryPtr : m_GeometriePtrs) {
		delete geometryPtr;
	}
}

void Scene::addObject(const Object &obj)
{
	m_Objects.push_back(obj);
}

void Scene::addLightSource(const LightSource &lightSource)
{
	m_LightSources.push_back(lightSource);
	std::for_each(m_Programs.begin(), m_Programs.end(), [this](Program &prog){prog.loadLights(m_LightSources); });
}

Geometry& Scene::addGeometryFromFile(const std::string &fileName)
{
	Geometry *newGeometry = new Geometry(fileName);
	m_GeometriePtrs.push_back(newGeometry);
	return *newGeometry;
}

Material& Scene::addMaterial(const Material &material)
{
	m_Materials.push_back(material);
	return m_Materials.back();
}

Program& Scene::addProgram(const Program &program)
{
	return (*((m_Programs.insert(std::make_pair(program.getId(), program))).first)).second;
}

void Scene::render()
{
	//call render on each object
	std::for_each(m_Objects.begin(), m_Objects.end(), [](const Object &obj){obj.render(); });
}
