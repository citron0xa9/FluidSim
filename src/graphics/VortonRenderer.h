#pragma once

#include "DrawableObject.h"

#include <vector>
#include <memory>
#include <shared_mutex>

#include "../simulation/Vorton.h"
#include "TriangleNetObject.h"

class VortonRenderer : public DrawableObject
{
public:
	VortonRenderer(Scene &scene, const std::vector<std::unique_ptr<Particle>> &baseVortonPtrs, std::shared_mutex& vortonVectorMutex, Program &phongProgram);
	virtual ~VortonRenderer();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

private:
	const std::vector<std::unique_ptr<Particle>> &m_BaseVortonPtrs;
    std::shared_mutex& m_VortonVectorMutex;
	TriangleNetObject m_DrawPrototype;
};

