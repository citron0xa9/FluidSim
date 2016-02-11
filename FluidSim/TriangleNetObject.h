#pragma once

#include "Object.h"
#include "Program.h"
#include "Geometry.h"
#include "Material.h"

class TriangleNetObject : public Object
{
public:
	TriangleNetObject(Scene& scene, Material& material, Geometry& geometry, Program& renderProg);
	virtual ~TriangleNetObject();

	virtual Object* copy() const override;

	virtual void render(const glm::mat4x4 &viewProjectTransform) const override;

	Program& getProgram() const;

private:

	Material& m_Material;
	Geometry& m_Geometry;
	Program& m_RenderProg;
};

