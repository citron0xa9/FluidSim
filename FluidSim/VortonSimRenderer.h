#pragma once

#include "DrawableObject.h"
#include "VortonSim.h"
#include "GridRenderer.h"
#include "VortonRenderer.h"
#include "TracerRenderer.h"

class VortonSimRenderer : public DrawableObject
{
public:
	VortonSimRenderer(const VortonSim &baseVortonSim, Scene &scene);
	virtual ~VortonSimRenderer();

	void tracersRendered(bool areRendered);
	void vortonsRendered(bool areRendered);
	void velocityGridRendered(bool isRendered);

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

private:
	const VortonSim &m_BaseVortonSim;
	Scene &m_Scene;

	GridRenderer *m_VelocityGridRendererPtr;
	VortonRenderer *m_VortonRendererPtr;
	TracerRenderer *m_TracerRendererPtr;
};

