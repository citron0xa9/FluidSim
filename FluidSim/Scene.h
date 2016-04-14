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

	virtual void render();

	void setAspectRatio(float ratio);

	Camera& getCamera();

private:
	void stepLoop();

	Camera m_Camera;
	//std::vector<Object*> m_ObjectPtrs;
	std::list<LightSource&> m_LightSourceRefs;

	std::list<Geometry*> m_GeometriePtrs;
	std::list<Material> m_Materials;
	std::unordered_map<GLuint, Program*> m_ProgramPtrs;

	std::thread m_StepLoopThread;
	bool m_Alive;
};

