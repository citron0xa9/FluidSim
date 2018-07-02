
#include "TracerRenderer.h"

#include "../util/fsmath.h"

TracerRenderer::TracerRenderer(const std::vector<std::unique_ptr<Particle>>& baseTracerPtrs, std::shared_mutex& tracerVectorMutex)
    : m_BaseTracerPtrs{ baseTracerPtrs }, m_TracerVectorMutex{ tracerVectorMutex }, m_TracerVerticesBuf {false}, m_TracerVao{ false }
{
    //updateVertexData();
	setupVao();
	setupRenderProgram();
}

TracerRenderer::~TracerRenderer()
{
}

void TracerRenderer::render(const glm::mat4x4 & viewProjectTransform)
{
    std::shared_lock<std::shared_mutex> vectorLock{ m_TracerVectorMutex };
    if (m_BaseTracerPtrs.empty()) {
        return;
    }

	updateVertexData();
	m_TracerVao.bind();

	m_RenderProgram.loadModelViewProjectTransform(viewProjectTransform);
	m_RenderProgram.use();

	glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_BaseTracerPtrs.size()));

	m_TracerVao.unbind();
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
	vertexShader.loadSourceFromFile("shaders/tracer.vert");

	Shader fragmentShader{ GL_FRAGMENT_SHADER };
	fragmentShader.loadSourceFromFile("shaders/tracer.frag");

	m_RenderProgram.attachShader(&vertexShader);
	m_RenderProgram.attachShader(&fragmentShader);

	vertexShader.compile();
	fragmentShader.compile();
	m_RenderProgram.link();
	m_RenderProgram.detachAllShaders();
}

void TracerRenderer::updateVertexData()
{
    if (m_BaseTracerPtrs.empty()) {
        return;
    }

    bool resized = false;
    const size_t neededVboSize = m_BaseTracerPtrs.size() * COMPONENTS_PER_TRACER;
    if (m_TracerVerticesRAM.size() < neededVboSize) {
        const size_t newVboSize = fsmath::nextLargerMultipleOf(TRACER_VBO_SIZE_STEP, neededVboSize);
        m_TracerVerticesRAM.resize(newVboSize);
        resized = true;
    }

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
    if (resized) {
        m_TracerVerticesBuf.pushData(m_TracerVerticesRAM, GL_DYNAMIC_DRAW, true);
    } else {
        m_TracerVerticesBuf.pushDataSubset(m_TracerVerticesRAM, 0, neededVboSize, true);
    }
}
