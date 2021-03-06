#include "Scene.h"
#include <chrono>
#include "util/Log.h"
#include "ActiveObject.h"
#include "graphics/DrawableObject.h"

const unsigned int Scene::m_STEP_TIME_MS = 15;

Scene::Scene() : m_Camera{*this}, m_Alive{true}
{
	m_Camera.registerSceneHooks(*this);
}


Scene::~Scene()
{
	m_Alive = false;
	m_StepLoopThread.join();

	m_GeometriePtrs.clear();
	m_ProgramPtrs.clear();
	m_ObjectPtrs.clear();
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
	loadLightsIntoPrograms();
}

Geometry& Scene::addGeometryFromFile(const std::string &fileName)
{
	m_GeometriePtrs.emplace_back(new Geometry{fileName});
	return *m_GeometriePtrs.back();
}

Material& Scene::addMaterial(const Material &material)
{
	m_Materials.push_back(material);
	return m_Materials.back();
}

Program & Scene::addProgram()
{
	m_ProgramPtrs.emplace_back(new Program{});
	return *m_ProgramPtrs.back();
}

Program& Scene::addProgram(const std::vector<ShaderLightSourceVariable> &lightVars)
{
	m_ProgramPtrs.emplace_back(new Program{lightVars});
	return *m_ProgramPtrs.back();
}

void Scene::render()
{
	glm::mat4x4 viewProjectTransform = m_Camera.viewPerspectiveTransform();
	glm::vec3 cameraPosition = m_Camera.position();

	for (auto& programPtr : m_ProgramPtrs) {
		programPtr->loadCameraPosition(cameraPosition);
	}
	
	render(viewProjectTransform);
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

void Scene::loadLightsIntoPrograms()
{
	for (auto& programPtr : m_ProgramPtrs) {
		programPtr->loadLights(m_LightSourcePtrs);
	}
}

void Scene::render(const glm::mat4x4 &viewProjectTransform)
{
	m_UsingObjectListsRender.lock();
	for (auto objectPtr : m_DrawableObjectPtrs) {
		objectPtr->render(viewProjectTransform);
	}
	m_UsingObjectListsRender.unlock();
}

void Scene::step(double secondsPassed)
{
	m_UsingObjectListsStep.lock();
	for (auto objectPtr : m_ActiveObjectPtrs) {
		objectPtr->step(secondsPassed);
	}
	m_UsingObjectListsStep.unlock();
}

Object& Scene::addObject(const Object & object)
{
	m_UsingObjectListsRender.lock();
	m_UsingObjectListsStep.lock();

	m_ObjectPtrs.emplace_back(object.copy());
	m_ObjectPtrs.back()->registerSceneHooks(*this);
	Object &addedObject = *m_ObjectPtrs.back();

	m_UsingObjectListsStep.unlock();
	m_UsingObjectListsRender.unlock();

	return addedObject;
}

Object* Scene::addObjectPtr(std::unique_ptr<Object>&& objPtr)
{
	m_UsingObjectListsRender.lock();
	m_UsingObjectListsStep.lock();

	m_ObjectPtrs.push_back(std::move(objPtr));
	m_ObjectPtrs.back()->registerSceneHooks(*this);
	auto objectPtr = m_ObjectPtrs.back().get();

	m_UsingObjectListsStep.unlock();
	m_UsingObjectListsRender.unlock();

	return objectPtr;
}

#if 0
void Scene::removeObjectPtr(Object *objectPtr)
{
	m_UsingObjectListsRender.lock();
	m_UsingObjectListsStep.lock();

	objectPtr->deregisterSceneHooks(*this);
	m_ObjectPtrs.remove(objectPtr);

	m_UsingObjectListsStep.unlock();
	m_UsingObjectListsRender.unlock();
}
#endif

void Scene::addActiveObject(ActiveObject & object)
{
	m_ActiveObjectPtrs.push_back(&object);
}

void Scene::removeActiveObject(ActiveObject & object)
{
	m_ActiveObjectPtrs.remove(&object);
}

void Scene::addDrawableObject(DrawableObject & object)
{
	m_DrawableObjectPtrs.push_back(&object);
}

void Scene::removeDrawableObject(DrawableObject & object)
{
	m_DrawableObjectPtrs.remove(&object);
}