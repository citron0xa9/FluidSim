#pragma once

#include <functional>
#include <memory>

#include "Program.h"
#include "VertexArrO.h"
#include "VertexBufO.h"
#include "DrawableObject.h"
#include "../simulation/UniformGridGeometry.h"

class GridRenderer : public DrawableObject
{
public:
    using grid_getter_t = std::function<const std::unique_ptr<UniformGridGeometry>&()>;
	GridRenderer(const grid_getter_t& gridGeometryGetter);
	virtual ~GridRenderer();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

	void updateGeometry(const UniformGridGeometry &geometry);

private:
	void setupRendering();
	void setupVao();
	void setupProgram();


	Program m_Program;
	std::vector<GLfloat> m_LinePointsRAM;
	VertexBufO m_LinePointsBuf;
	VertexArrO m_LinesVao;

	glm::vec3 m_Color;

	static const GLuint m_VertexPositionIndex;
	static const std::string m_ColorUniformName;

	grid_getter_t m_GridGeometryGetter;
};

