#pragma once

#include "DrawableObject.h"
#include "VortonSim.h"
#include "GridRenderer.h"
#include "VortonRenderer.h"
#include "TracerRenderer.h"
#include "VectorFieldRenderer.h"

class VortonSimRenderer : public DrawableObject
{
public:
	VortonSimRenderer(const VortonSim &baseVortonSim, Scene &scene);
	virtual ~VortonSimRenderer();

	void tracersRendered(bool areRendered);
	void vortonsRendered(bool areRendered);
	void velocityGridRendered(bool isRendered);
	void velocityVectorsRendered(bool isRendered);

	VectorFieldRenderer* VelocityVectorsRendererPtr();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

private:
	void setupPhongProgram(Scene &scene);

	const VortonSim &m_BaseVortonSim;
	Scene &m_Scene;

	GridRenderer *m_VelocityGridRendererPtr;
	VectorFieldRenderer *m_VelocityVectorsRendererPtr;
	VortonRenderer *m_VortonRendererPtr;
	TracerRenderer *m_TracerRendererPtr;

	Program *m_PhongProgramPtr;
};

