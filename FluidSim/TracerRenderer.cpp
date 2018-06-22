
#include "TracerRenderer.h"

TracerRenderer::TracerRenderer(const std::vector<std::unique_ptr<Particle>>& baseTracerPtrs)
	: m_BaseTracerPtrs{baseTracerPtrs}, m_TracerVerticesBuf{false}, m_TracerVao{false}
{
	assert(!baseTracers.empty());
	setupVertexData();
	setupVao();
	setupRenderProgram();
}

TracerRenderer::~TracerRenderer()
{
}

void TracerRenderer::render(const glm::mat4x4 & viewProjectTransform)
{
	updateVertexData();
	m_TracerVao.bind();

	m_RenderProgram.loadModelViewProjectTransform(viewProjectTransform);
	m_RenderProgram.use();

	glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_BaseTracerPtrs.size()));

	m_TracerVao.unbind();
}

void TracerRenderer::setupVertexData()
{
	for (auto &tracerPtr : m_BaseTracerPtrs) {
		glm::dvec3 position = tracerPtr->position();
		for (int i = 0; i < position.length(); i++) {
			m_TracerVerticesRAM.push_back(position[i]);
		}
	}
	m_TracerVerticesBuf.pushData(m_TracerVerticesRAM, GL_DYNAMIC_DRAW, true);
}

void TracerRenderer::setupVao()
{
	m_TracerVao.addVertexAttribArray(m_TracerVerticesBuf, true, true, m_RenderProgram.vertexPositionIndex(), 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
	m_TracerVao.enableVertexAttribArray(false, m_RenderProgram.vertexPositionIndex());
	m_TracerVao.unbind();
}

void TracerRenderer::setupRenderProgram()
{
	Shader vertexShader{ GL_VERTEX_SHADER };
	vertexShader.loadSourceFromFile("shaders\\tracer.vert");

	Shader fragmentShader{ GL_FRAGMENT_SHADER };
	fragmentShader.loadSourceFromFile("shaders\\tracer.frag");

	m_RenderProgram.attachShader(&vertexShader);
	m_RenderProgram.attachShader(&fragmentShader);

	vertexShader.compile();
	fragmentShader.compile();
	m_RenderProgram.link();
	m_RenderProgram.detachAllShaders();
}

void TracerRenderer::updateVertexData()
{
	assert(m_BaseTracers.size() == (m_TracerVerticesRAM.size() / 3));
	{
		size_t verticesRAMOffset = 0;
		for (int i = 0; i < m_BaseTracerPtrs.size(); i++) {
			glm::dvec3 position = m_BaseTracerPtrs[i]->position();
			for (int j = 0; j < position.length(); j++) {
				m_TracerVerticesRAM[verticesRAMOffset] = position[j];
				verticesRAMOffset++;
			}
		}
	}
	m_TracerVerticesBuf.pushDataSubset(m_TracerVerticesRAM, 0, m_TracerVerticesRAM.size(), true);
}
