#pragma once

#include "DrawableObject.h"
#include "VortonSim.h"
#include "GridRenderer.h"
#include "VortonRenderer.h"
#include "TracerRenderer.h"
#include "VectorFieldRenderer.h"
#include "VortonOctHeapRenderer.h"
#include "VortonRendererOctHeap.h"

class VortonSimRenderer : public DrawableObject
{
public:
	VortonSimRenderer(const VortonSim &baseVortonSim, Scene &scene);
	virtual ~VortonSimRenderer();

	void tracersRendered(bool areRendered);
	void vortonsRendered(bool areRendered);
	void velocityGridRendered(bool isRendered);
	void velocityVectorsRendered(bool isRendered);
	void vortonOctHeapRendered(bool isRendered);
	void vortonsOctHeapRendered(bool isRendered);

	VectorFieldRenderer* VelocityVectorsRendererPtr();
	VortonOctHeapRenderer* vortonOctHeapRendererPtr();
	VortonRendererOctHeap* vortonRendererOctHeapPtr();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

private:
	void setupPhongProgram(Scene &scene);

	const VortonSim &m_BaseVortonSim;
	Scene &m_Scene;

	std::unique_ptr<GridRenderer> m_VelocityGridRendererPtr;
	std::unique_ptr<VectorFieldRenderer> m_VelocityVectorsRendererPtr;
	std::unique_ptr<VortonRenderer> m_VortonRendererPtr;
	std::unique_ptr<TracerRenderer> m_TracerRendererPtr;
	std::unique_ptr<VortonOctHeapRenderer> m_VortonOctHeapRendererPtr;
	std::unique_ptr<VortonRendererOctHeap> m_VortonRendererOctHeapPtr;

	Program *m_PhongProgramPtr;
};

