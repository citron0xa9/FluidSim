#pragma once

#include "DrawableObject.h"

#include <vector>

#include "Vorton.h"
#include "TriangleNetObject.h"

class VortonRenderer : public DrawableObject
{
public:
	VortonRenderer(Scene &scene, const std::vector<Vorton> &baseVortons, Program &phongProgram);
	virtual ~VortonRenderer();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

private:
	const std::vector<Vorton> &m_BaseVortons;
	TriangleNetObject m_DrawPrototype;
};

