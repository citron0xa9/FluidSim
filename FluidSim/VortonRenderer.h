#pragma once

#include "DrawableObject.h"

#include <vector>

#include "Vorton.h"
#include "TriangleNetObject.h"

class VortonRenderer : public DrawableObject
{
public:
	VortonRenderer(Scene &scene, const std::vector<std::unique_ptr<Particle>> &baseVortonPtrs, Program &phongProgram);
	virtual ~VortonRenderer();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

private:
	const std::vector<std::unique_ptr<Particle>> &m_BaseVortonPtrs;
	TriangleNetObject m_DrawPrototype;
};

