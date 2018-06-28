#pragma once

#include "DrawableObject.h"
#include "../simulation/Particle.h"
#include <vector>
#include <memory>

#include "VertexBufO.h"
#include "VertexArrO.h"
#include "Program.h"

class TracerRenderer : public DrawableObject
{
public:
	TracerRenderer(const std::vector<std::unique_ptr<Particle>>& baseTracerPtrs);
	virtual ~TracerRenderer();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

private:
	void setupVao();
	void setupRenderProgram();

	void updateVertexData();

	const std::vector<std::unique_ptr<Particle>> &m_BaseTracerPtrs;

	std::vector<GLfloat> m_TracerVerticesRAM;
    static constexpr size_t COMPONENTS_PER_TRACER = 3u;
    static constexpr size_t TRACER_VBO_SIZE_STEP = 2048 * COMPONENTS_PER_TRACER;
	VertexBufO m_TracerVerticesBuf;
	VertexArrO m_TracerVao;
	Program m_RenderProgram;
};

