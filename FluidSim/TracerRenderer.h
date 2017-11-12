#pragma once

#include "DrawableObject.h"
#include "Tracer.h"
#include <vector>

#include "VertexBufO.h"
#include "VertexArrO.h"
#include "Program.h"

class TracerRenderer : public DrawableObject
{
public:
	TracerRenderer(const std::vector<Tracer> &baseTracers);
	virtual ~TracerRenderer();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

private:
	void setupVertexData();
	void setupVao();
	void setupRenderProgram();

	void updateVertexData();

	const std::vector<Tracer> &m_BaseTracers;

	std::vector<GLfloat> m_TracerVerticesRAM;
	VertexBufO m_TracerVerticesBuf;
	VertexArrO m_TracerVao;
	Program m_RenderProgram;
};

