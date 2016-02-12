#include "Scene.h"
#include <chrono>

Scene::Scene() : m_Camera{*this}, m_Alive{true}
{
	m_StepLoopThread = std::thread{ &Scene::stepLoop, this };
}


Scene::~Scene()
{
	m_Alive = false;
	m_StepLoopThread.join();

	for (auto geometryPtr : m_GeometriePtrs) {
		delete geometryPtr;
	}
	for (auto &elem : m_ProgramPtrs) {
		delete elem.second;
	}
	for (auto &elem : m_LightSources) {
		delete elem;
	}
	for (auto &elem : m_ObjectPtrs) {
		delete elem;
	}
}

void Scene::stepLoop()
{
	int stepTimeMilliseconds = 15;
	while (m_Alive) {
		std::this_thread::sleep_for(std::chrono::milliseconds(stepTimeMilliseconds));
		step(stepTimeMilliseconds / 1000.0);
	}
}

void Scene::addObject(const Object &obj)
{
	m_ObjectPtrs.push_back(obj.copy());
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

	//glm::vec3 lookDirection = m_Camera.getLookDirection();
	//std::for_each(m_ProgramPtrs.begin(), m_ProgramPtrs.end(), [&lookAt](std::pair<const GLuint, Program*> &elem){(elem.second)->loadCameraLookDirection(lookDirection); });
	std::for_each(m_ObjectPtrs.begin(), m_ObjectPtrs.end(), [&viewProjectTransform](const Object *obj){obj->render(viewProjectTransform); });
}

void Scene::setAspectRatio(float ratio)
{
	m_Camera.setAspectRatio(ratio);
}

Camera& Scene::getCamera()
{
	return m_Camera;
}

void Scene::step(float secondsPassed)
{
	m_Camera.step(secondsPassed);
	for (auto & obj : m_ObjectPtrs) {
		obj->step(secondsPassed);
	}
}
