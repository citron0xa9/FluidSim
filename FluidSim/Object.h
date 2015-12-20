#pragma once

#include <glm/mat4x4.hpp>
//#include <string>
#include "Program.h"
#include "Geometry.h"
#include "Material.h"
#include "Scene.h"

class Object
{
public:
	Object(Scene& scene, Material& material, Geometry& geometry, Program& renderProg);
	virtual ~Object();

	void render() const;

	Program& getProgram() const;
private:
	//std::string m_Name;

	Scene& m_Scene;
	Material& m_Material;
	Geometry& m_Geometry;
	glm::mat4x4 m_ModelTransform;
	Program& m_RenderProg;
};

