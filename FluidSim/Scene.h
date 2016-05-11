#pragma once

#include <vector>
#include <unordered_set>
#include <thread>
#include "Object.h"
#include "Geometry.h"
#include "LightSource.h"
#include "Program.h"
#include "Material.h"
#include "Camera.h"
#include "ContainerObject.h"

class Scene : public ContainerObject
{
public:
	Scene();
	virtual ~Scene();

	void addLightSource(LightSource &lightSource);
	Geometry& addGeometryFromFile(const std::string &fileName);
	Material& addMaterial(const Material &material);
	Program& addProgram(const std::vector<ShaderLightSourceVariable> &lightVars);

	//Geometry& firstGeometry();
	//Material& firstMaterial();
	//Program& firstProgram();

	virtual void render();

	void aspectRatio(float ratio);

	Camera& camera();

	void startStepping();

private:
	void stepLoop();

	Camera m_Camera;
	std::list<LightSource*> m_LightSourcePtrs;

	std::list<Geometry*> m_GeometriePtrs;
	std::list<Material> m_Materials;
	std::list<Program*> m_ProgramPtrs;

	std::thread m_StepLoopThread;
	bool m_Alive;

	static const unsigned int m_STEP_TIME_MS;
};

