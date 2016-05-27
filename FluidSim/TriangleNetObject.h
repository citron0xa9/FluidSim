#pragma once

#include "DrawableObject.h"
#include "Program.h"
#include "Geometry.h"
#include "Material.h"

class TriangleNetObject : public DrawableObject
{
public:
	TriangleNetObject(Material *materialPtr = nullptr, Geometry *geometryPtr = nullptr, Program *renderProgPtr = nullptr);
	virtual ~TriangleNetObject();

	virtual Object* copy() const override;

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

	const Program& program() const;
	void program(Program *programPtr);

	const Material& material() const;
	void material(Material *materialPtr);

	const Geometry& geometry() const;
	void geometry(Geometry *geometryPtr);

private:
	Material *m_MaterialPtr;
	Geometry *m_GeometryPtr;
	Program *m_ProgramPtr;
};

