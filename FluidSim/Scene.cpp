#include "Scene.h"
#include <chrono>

Scene::Scene() : m_Camera{*this}, m_Alive{true}
{
	m_Camera.registerContainerObjectHooks();
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
}

void Scene::stepLoop()
{
	int stepTimeMilliseconds = 15;
	while (m_Alive) {
		std::this_thread::sleep_for(std::chrono::milliseconds(stepTimeMilliseconds));
		step(stepTimeMilliseconds / 1000.0f);
	}
}

void Scene::addLightSource(LightSource &lightSource)
{
	m_LightSourcePtrs.push_back(&lightSource);
	std::for_each(m_ProgramPtrs.begin(), m_ProgramPtrs.end(), [this](std::pair<const GLuint, Program*> &elem){(elem.second)->loadLights(m_LightSourcePtrs); });
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
	return *((*((m_ProgramPtrs.insert(std::make_pair(newProg->id(), newProg))).first)).second);
}

void Scene::render()
{
	//call render on each object
	glm::mat4x4 viewProjectTransform = m_Camera.viewPerspectiveTransform();

	glm::vec3 camPosition = m_Camera.getPosition();
	std::for_each(m_ProgramPtrs.begin(), m_ProgramPtrs.end(), [&camPosition](std::pair<const GLuint, Program*> &elem){(elem.second)->loadCameraPosition(camPosition); });
	
	ContainerObject::render(viewProjectTransform);
}


void Scene::aspectRatio(float ratio)
{
	m_Camera.aspectRatio(ratio);
}

Camera& Scene::getCamera()
{
	return m_Camera;
}

void Scene::startStepping()
{
	m_StepLoopThread = std::thread{ &Scene::stepLoop, this };
}
