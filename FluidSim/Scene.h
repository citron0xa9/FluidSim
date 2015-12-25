#pragma once

#include <vector>
#include <unordered_set>
#include "Object.h"
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
	Program& addProgram(const Program& Program);

	void render();
	void setAspectRatio(float ratio);

private:
	Camera m_Camera;
	std::vector<Object> m_Objects;
	std::vector<LightSource> m_LightSources;

	std::list<Geometry*> m_GeometriePtrs;
	std::list<Material> m_Materials;
	std::unordered_map<GLuint, Program> m_Programs;
};

