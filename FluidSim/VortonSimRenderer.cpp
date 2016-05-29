
#include "VortonSimRenderer.h"

VortonSimRenderer::VortonSimRenderer(const VortonSim & baseVortonSim, Scene & scene)
	: m_BaseVortonSim{ baseVortonSim }, m_Scene{ scene },
	m_TracerRendererPtr{ nullptr }, m_VortonRendererPtr{ nullptr }, m_VelocityGridRendererPtr{ nullptr }
{
	m_TracerRendererPtr = new TracerRenderer{ baseVortonSim.tracers() };
}

VortonSimRenderer::~VortonSimRenderer()
{
}

void VortonSimRenderer::tracersRendered(bool areRendered)
{
	if (areRendered) {
		if (m_TracerRendererPtr == nullptr) {
			//not rendered before
			m_TracerRendererPtr = new TracerRenderer{ m_BaseVortonSim.tracers() };
		}
	}
	else {
		if (m_TracerRendererPtr != nullptr) {
			//was rendered before
			delete m_TracerRendererPtr;
			m_TracerRendererPtr = nullptr;
		}
	}
}

void VortonSimRenderer::vortonsRendered(bool areRendered)
{
	if (areRendered) {
		if (m_VortonRendererPtr == nullptr) {
			//not rendered before
			m_VortonRendererPtr = new VortonRenderer{ m_Scene, m_BaseVortonSim.vortons() };
		}
	}
	else {
		if (m_VortonRendererPtr != nullptr) {
			//was rendered before
			delete m_VortonRendererPtr;
			m_VortonRendererPtr = nullptr;
		}
	}
}

void VortonSimRenderer::velocityGridRendered(bool isRendered)
{
	if (isRendered) {
		if (m_VelocityGridRendererPtr == nullptr) {
			//not rendered before
			m_VelocityGridRendererPtr = new GridRenderer{ std::bind(&VortonSim::velocityGridPtr, &m_BaseVortonSim) };
		}
	}
	else {
		if (m_VelocityGridRendererPtr != nullptr) {
			//was rendered before
			delete m_VelocityGridRendererPtr;
			m_VelocityGridRendererPtr = nullptr;
		}
	}
}

void VortonSimRenderer::render(const glm::mat4x4 & viewProjectTransform)
{
	std::vector<DrawableObject*> renderers{ m_TracerRendererPtr, m_VortonRendererPtr, m_VelocityGridRendererPtr };
	for (auto renderer : renderers) {
		if (renderer != nullptr) {
			renderer->render(viewProjectTransform);
		}
	}
}
