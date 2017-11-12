
#include "VortonSimRenderer.h"
#include "Scene.h"

VortonSimRenderer::VortonSimRenderer(const VortonSim & baseVortonSim, Scene & scene)
	: m_BaseVortonSim{ baseVortonSim }, m_Scene{ scene },
	m_TracerRendererPtr{ nullptr }, m_VortonRendererPtr{ nullptr }, m_VelocityGridRendererPtr{ nullptr },
	m_VortonOctHeapRendererPtr{nullptr}
{
	m_TracerRendererPtr = std::unique_ptr<TracerRenderer>(new TracerRenderer{ baseVortonSim.tracers() });

	setupPhongProgram(scene);
}

VortonSimRenderer::~VortonSimRenderer()
{
}

void VortonSimRenderer::tracersRendered(bool areRendered)
{
	if (areRendered) {
		if (m_TracerRendererPtr == nullptr) {
			//not rendered before
			m_TracerRendererPtr = std::unique_ptr<TracerRenderer>(new TracerRenderer{ m_BaseVortonSim.tracers() });
		}
	}
	else {
		if (m_TracerRendererPtr != nullptr) {
			//was rendered before
			m_TracerRendererPtr = nullptr;
		}
	}
}

void VortonSimRenderer::vortonsRendered(bool areRendered)
{
	if (areRendered) {
		if (m_VortonRendererPtr == nullptr) {
			//not rendered before
			m_VortonRendererPtr =
				std::unique_ptr<VortonRenderer>(new VortonRenderer{ m_Scene, m_BaseVortonSim.vortons(), *m_PhongProgramPtr });
		}
	}
	else {
		if (m_VortonRendererPtr != nullptr) {
			//was rendered before
			m_VortonRendererPtr = nullptr;
		}
	}
}

void VortonSimRenderer::velocityGridRendered(bool isRendered)
{
	if (isRendered) {
		if (m_VelocityGridRendererPtr == nullptr) {
			//not rendered before
			m_VelocityGridRendererPtr = 
				std::unique_ptr<GridRenderer>(new GridRenderer{ std::bind(&VortonSim::velocityGridPtr, &m_BaseVortonSim) });
		}
	}
	else {
		if (m_VelocityGridRendererPtr != nullptr) {
			//was rendered before
			m_VelocityGridRendererPtr = nullptr;
		}
	}
}

void VortonSimRenderer::velocityVectorsRendered(bool isRendered)
{
	if (isRendered) {
		if (m_VelocityVectorsRendererPtr == nullptr) {
			//not rendered before
			m_VelocityVectorsRendererPtr = 
				std::unique_ptr<VectorFieldRenderer>(
					new VectorFieldRenderer{ m_Scene, std::bind(&VortonSim::velocityGridPtr, &m_BaseVortonSim), *m_PhongProgramPtr }
				);
            m_VelocityVectorsRendererPtr->renderLines(false);
        } else {
            m_VelocityVectorsRendererPtr->renderArrows(true);
        }
	}
	else {
		if (m_VelocityVectorsRendererPtr != nullptr) {
			//was rendered before
            m_VelocityVectorsRendererPtr->renderArrows(false);
            if (!m_VelocityVectorsRendererPtr->isRenderingLines()) {
                m_VelocityVectorsRendererPtr = nullptr;
            }
		}
	}
}

void VortonSimRenderer::velocityLinesRendered(bool isRendered)
{
    if (isRendered) {
        if (m_VelocityVectorsRendererPtr == nullptr) {
            //not rendered before
            m_VelocityVectorsRendererPtr =
                std::unique_ptr<VectorFieldRenderer>(
                    new VectorFieldRenderer{ m_Scene, std::bind(&VortonSim::velocityGridPtr, &m_BaseVortonSim), *m_PhongProgramPtr }
            );
            m_VelocityVectorsRendererPtr->renderArrows(false);
        } else {
            m_VelocityVectorsRendererPtr->renderLines(true);
        }
    } else {
        if (m_VelocityVectorsRendererPtr != nullptr) {
            //was rendered before
            m_VelocityVectorsRendererPtr->renderLines(false);
            if (!m_VelocityVectorsRendererPtr->isRenderingArrows()) {
                m_VelocityVectorsRendererPtr = nullptr;
            }
        }
    }
}

void VortonSimRenderer::vortonOctHeapRendered(bool isRendered)
{
	if (isRendered) {
		if (m_VortonOctHeapRendererPtr == nullptr) {
			//not rendered before
			m_VortonOctHeapRendererPtr =
				std::unique_ptr<VortonOctHeapRenderer>(
					new VortonOctHeapRenderer{ std::bind(&VortonSim::vortonOctHeapPtr, &m_BaseVortonSim)}
				);
		}
	}
	else {
		if (m_VortonOctHeapRendererPtr != nullptr) {
			//was rendered before
			m_VortonOctHeapRendererPtr = nullptr;
		}
	}
}

void VortonSimRenderer::vortonsOctHeapRendered(bool isRendered)
{
	if (isRendered) {
		if (m_VortonRendererOctHeapPtr == nullptr) {
			//not rendered before
			m_VortonRendererOctHeapPtr =
				std::unique_ptr<VortonRendererOctHeap>(
					new VortonRendererOctHeap{ m_Scene, *m_PhongProgramPtr, std::bind(&VortonSim::vortonOctHeapPtr, &m_BaseVortonSim) }
			);
		}
	}
	else {
		if (m_VortonRendererOctHeapPtr != nullptr) {
			//was rendered before
			m_VortonRendererOctHeapPtr = nullptr;
		}
	}
}

VectorFieldRenderer * VortonSimRenderer::VelocityVectorsRendererPtr()
{
	return m_VelocityVectorsRendererPtr.get();
}

VortonOctHeapRenderer * VortonSimRenderer::vortonOctHeapRendererPtr()
{
	return m_VortonOctHeapRendererPtr.get();
}

VortonRendererOctHeap * VortonSimRenderer::vortonRendererOctHeapPtr()
{
	return m_VortonRendererOctHeapPtr.get();
}

void VortonSimRenderer::render(const glm::mat4x4 & viewProjectTransform)
{
	std::vector<DrawableObject*> renderers{ m_TracerRendererPtr.get(), m_VortonRendererPtr.get(), m_VelocityGridRendererPtr.get(),
		m_VelocityVectorsRendererPtr.get(), m_VortonOctHeapRendererPtr.get(), m_VortonRendererOctHeapPtr.get() };
	for (auto renderer : renderers) {
		if (renderer != nullptr) {
			renderer->render(viewProjectTransform);
		}
	}
}

void VortonSimRenderer::setupPhongProgram(Scene &scene)
{
	ShaderLightSourceVariable lightSrcVar{ "sunLight", LightSourceType::SUNLIGHT };
	std::vector<ShaderLightSourceVariable> lightSrcVars;
	lightSrcVars.push_back(lightSrcVar);

	m_PhongProgramPtr = &(scene.addProgram(lightSrcVars));

	Shader vertexShader{ GL_VERTEX_SHADER };
	vertexShader.loadSourceFromFile("shaders\\basic3D.vert");

	Shader fragmentShader{ GL_FRAGMENT_SHADER };
	fragmentShader.loadSourceFromFile("shaders\\basic3D_phong.frag");

	m_PhongProgramPtr->attachShader(&vertexShader);
	m_PhongProgramPtr->attachShader(&fragmentShader);

	vertexShader.compile();
	fragmentShader.compile();
	m_PhongProgramPtr->link();
	m_PhongProgramPtr->detachAllShaders();

	scene.loadLightsIntoPrograms();
}
