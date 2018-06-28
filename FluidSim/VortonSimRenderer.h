#pragma once

#include "DrawableObject.h"
#include "GridRenderer.h"
#include "VortonRenderer.h"
#include "TracerRenderer.h"
#include "VectorFieldRenderer.h"
#include "VortonOctHeapRenderer.h"
#include "VortonRendererOctHeap.h"

#include "ParticleSystem.h"
#include "UpdateFluidOperation.h"

class VortonSimRenderer : public DrawableObject
{
public:
	VortonSimRenderer(
        const ParticleSystem& vortonParticleSystem,
        const ParticleSystem& tracerParticleSystem,
        const UpdateFluidOperation& updateFuildOperation,
        Scene &scene);
	virtual ~VortonSimRenderer();

	void tracersRendered(bool areRendered);
	void vortonsRendered(bool areRendered);
	void velocityGridRendered(bool isRendered);
	void velocityVectorsRendered(bool isRendered);
    void velocityLinesRendered(bool isRendered);
	void vortonOctHeapRendered(bool isRendered);
	void vortonsOctHeapRendered(bool isRendered);

	VectorFieldRenderer* VelocityVectorsRendererPtr();
	VortonOctHeapRenderer* vortonOctHeapRendererPtr();
	VortonRendererOctHeap* vortonRendererOctHeapPtr();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

private:
	void setupPhongProgram(Scene &scene);

	const ParticleSystem& m_VortonParticleSystem;
    const ParticleSystem& m_TracerParticleSystem;
    const UpdateFluidOperation& m_UpdateFluidOperation;
	Scene &m_Scene;

	std::unique_ptr<GridRenderer> m_VelocityGridRendererPtr;
	std::unique_ptr<VectorFieldRenderer> m_VelocityVectorsRendererPtr;
	std::unique_ptr<VortonRenderer> m_VortonRendererPtr;
	std::unique_ptr<TracerRenderer> m_TracerRendererPtr;
	std::unique_ptr<VortonOctHeapRenderer> m_VortonOctHeapRendererPtr;
	std::unique_ptr<VortonRendererOctHeap> m_VortonRendererOctHeapPtr;

	Program *m_PhongProgramPtr;
};

