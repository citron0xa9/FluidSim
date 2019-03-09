#pragma once

#include <functional>
#include <memory>
#include <shared_mutex>

#include "../simulation/VortonOctHeap.h"
#include "DrawableObject.h"
#include "Program.h"
#include "VertexArrO.h"

class VortonOctHeapRenderer : public DrawableObject
{
public:
using oct_heap_getter_t = std::function<const std::unique_ptr<VortonOctHeap>&(void)>;

	VortonOctHeapRenderer(const oct_heap_getter_t& octHeapGetter, std::shared_mutex& octHeapMutex);
	virtual ~VortonOctHeapRenderer();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

	void updateGeometry(const VortonOctHeap &octHeap);

	void renderedLevel(size_t level);

private:
	void setupRendering();
	void setupVao();
	void setupProgram();

	size_t m_RenderedLevel;


	Program m_Program;
	std::vector<GLfloat> m_LinePointsRAM;
	VertexBufO m_LinePointsBuf;
	VertexArrO m_LinesVao;

	glm::vec3 m_Color;

	static const GLuint m_VertexPositionIndex;
	static const std::string m_ColorUniformName;

    oct_heap_getter_t m_OctHeapGetter;
    std::shared_mutex& m_OctHeapMutex;
};

