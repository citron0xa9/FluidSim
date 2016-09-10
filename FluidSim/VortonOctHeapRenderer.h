#pragma once

#include <functional>
#include <memory>
#include "VortonOctHeap.h"

class VortonOctHeapRenderer : public DrawableObject
{
public:
	VortonOctHeapRenderer(std::function<const std::shared_ptr<VortonOctHeap>(void)> octHeapGetter);
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

	std::function<const std::shared_ptr<VortonOctHeap>(void)> m_OctHeapGetter;
};

