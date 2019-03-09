#pragma once

#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>
#include "Object.h"
#include "graphics/DrawableObject.h"
#include "ActiveObject.h"
#include "graphics/Geometry.h"
#include "graphics/LightSource.h"
#include "graphics/Program.h"
#include "graphics/Material.h"
#include "graphics/Camera.h"

class Scene
{
public:
	Scene();
	virtual ~Scene();

	void addLightSource(LightSource &lightSource);
	Geometry& addGeometryFromFile(const std::string &fileName);
	Material& addMaterial(const Material &material);
	Program& addProgram();
	Program& addProgram(const std::vector<ShaderLightSourceVariable> &lightVars);

	virtual void render();
	virtual void render(const glm::mat4x4 &viewProjectTransform);
	virtual void step(double secondsPassed);

	Object& addObject(const Object &object);
	Object* addObjectPtr(std::unique_ptr<Object>&& objPtr);
	//void removeObjectPtr(Object *objectPtr);

	void addActiveObject(ActiveObject &object);
	void removeActiveObject(ActiveObject &object);

	void addDrawableObject(DrawableObject &object);
	void removeDrawableObject(DrawableObject &object);

	void aspectRatio(float ratio);

	Camera& camera();

	void startStepping();

	void loadLightsIntoPrograms();

private:
	void stepLoop();

	Camera m_Camera;
	std::list<LightSource*> m_LightSourcePtrs;

	std::list<std::unique_ptr<Geometry>> m_GeometriePtrs;
	std::list<Material> m_Materials;
	std::list<std::unique_ptr<Program>> m_ProgramPtrs;

	std::thread m_StepLoopThread;
	bool m_Alive;

	static const unsigned int m_STEP_TIME_MS;

	std::list<std::unique_ptr<Object>> m_ObjectPtrs;
	std::list<ActiveObject*> m_ActiveObjectPtrs;
	std::list<DrawableObject*> m_DrawableObjectPtrs;

	std::mutex m_UsingObjectListsStep;
	std::mutex m_UsingObjectListsRender;
};

