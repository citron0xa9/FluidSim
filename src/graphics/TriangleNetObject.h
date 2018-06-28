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
	void renderWithId(const glm::mat4x4 &viewProjectTransform, unsigned int id);

	const Program& program() const;
	Program& program();
	void program(Program *programPtr);

	const Material& material() const;
	Material& material();
	void material(Material *materialPtr);

	const Geometry& geometry() const;
	void geometry(Geometry *geometryPtr);

private:
	void coreRender(const glm::mat4x4 &viewProjectTransform);

	Material *m_MaterialPtr;
	Geometry *m_GeometryPtr;
	Program *m_ProgramPtr;
};

