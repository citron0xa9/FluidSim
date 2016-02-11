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

class Scene
{
public:
	Scene();
	virtual ~Scene();

	void addObject(const Object &obj);
	void addLightSource(const LightSource &lightSource);
	Geometry& addGeometryFromFile(const std::string &fileName);
	Material& addMaterial(const Material &material);
	Program& addProgram(const std::vector<ShaderLightSourceVariable> &lightVars);

	void render();
	void setAspectRatio(float ratio);

	Camera& getCamera();

private:
	void step(float secondsPassed);
	void stepLoop();

	Camera m_Camera;
	std::vector<Object*> m_ObjectPtrs;
	std::vector<LightSource*> m_LightSources;

	std::list<Geometry*> m_GeometriePtrs;
	std::list<Material> m_Materials;
	std::unordered_map<GLuint, Program*> m_ProgramPtrs;

	std::thread m_StepLoopThread;
	bool m_Alive;
};

