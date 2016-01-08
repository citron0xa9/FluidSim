#include "Scene.h"


Scene::Scene() : m_Camera{}
{

}


Scene::~Scene()
{
	for (auto geometryPtr : m_GeometriePtrs) {
		delete geometryPtr;
	}
	for (auto &elem : m_ProgramPtrs) {
		delete elem.second;
	}
	for (auto &elem : m_LightSources) {
		delete elem;
	}
}

void Scene::addObject(const Object &obj)
{
	m_Objects.push_back(obj);
}

void Scene::addLightSource(const LightSource &lightSource)
{
	m_LightSources.push_back(lightSource.clone());
	std::for_each(m_ProgramPtrs.begin(), m_ProgramPtrs.end(), [this](std::pair<const GLuint, Program*> &elem){(elem.second)->loadLights(m_LightSources); });
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

Program& Scene::addProgram(const std::vector<ShaderLightSourceVariable> &lightVars)
{
	Program *newProg = new Program{ lightVars };
	return *((*((m_ProgramPtrs.insert(std::make_pair(newProg->getId(), newProg))).first)).second);
}

void Scene::render()
{
	//call render on each object
	glm::mat4x4 viewProjectTransform = m_Camera.getViewPerspectiveTransform();
	glm::vec3 lookAt = m_Camera.getLookAt();
	std::for_each(m_ProgramPtrs.begin(), m_ProgramPtrs.end(), [&lookAt](std::pair<const GLuint, Program*> &elem){(elem.second)->loadCameraLookAt(lookAt); });
	std::for_each(m_Objects.begin(), m_Objects.end(), [&viewProjectTransform](const Object &obj){obj.render(viewProjectTransform); });
}

void Scene::setAspectRatio(float ratio)
{
	m_Camera.setAspectRatio(ratio);
}