#include "Scene.h"
#include <chrono>
#include "Log.h"

const unsigned int Scene::m_STEP_TIME_MS = 15;

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
	for (auto &programPtr : m_ProgramPtrs) {
		delete programPtr;
	}
}

void Scene::stepLoop()
{
	static const unsigned int runningBehindThresholdFactor = 10;

	auto timeBeforeLastStep = std::chrono::steady_clock::now();
	while (m_Alive) {

		double secondsPassed = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timeBeforeLastStep).count()) / 1000.0;
		if ((secondsPassed*1000) > (runningBehindThresholdFactor * m_STEP_TIME_MS)) {
			WARNING(std::string("stepLoop: Step takes to long! (took ") + std::to_string(secondsPassed) + std::string(" seconds)"));
			secondsPassed = m_STEP_TIME_MS / 1000.0;
		}

		int leftMilliseconds = m_STEP_TIME_MS - static_cast<unsigned int>(secondsPassed * 1000);
		if (leftMilliseconds > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(m_STEP_TIME_MS));
			secondsPassed = m_STEP_TIME_MS / 1000.0;
		}

		timeBeforeLastStep = std::chrono::steady_clock::now();
		step(secondsPassed);
	}
}

void Scene::addLightSource(LightSource &lightSource)
{
	m_LightSourcePtrs.push_back(&lightSource);
	for (auto programPtr : m_ProgramPtrs) {
		programPtr->loadLights(m_LightSourcePtrs);
	}
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
	m_ProgramPtrs.push_back(newProg);
	return *m_ProgramPtrs.back();
}

/*Geometry & Scene::firstGeometry()
{
	return *m_GeometriePtrs.front();
}

Material & Scene::firstMaterial()
{
	return m_Materials.front();
}

Program & Scene::firstProgram()
{
	return *m_ProgramPtrs.front();
}*/

void Scene::render()
{
	glm::mat4x4 viewProjectTransform = m_Camera.viewPerspectiveTransform();
	glm::vec3 cameraPosition = m_Camera.position();

	for (auto programPtr : m_ProgramPtrs) {
		programPtr->loadCameraPosition(cameraPosition);
	}
	
	ContainerObject::render(viewProjectTransform);
}


void Scene::aspectRatio(float ratio)
{
	m_Camera.aspectRatio(ratio);
}

Camera& Scene::camera()
{
	return m_Camera;
}

void Scene::startStepping()
{
	m_StepLoopThread = std::thread{ &Scene::stepLoop, this };
}
