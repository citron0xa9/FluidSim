#pragma once

#include "Program.h"
#include "VertexArrO.h"
#include "VertexBufO.h"
#include "DrawableObject.h"
#include "UniformGridGeometry.h"

class DrawableGridGeometry : public DrawableObject
{
public:
	DrawableGridGeometry(ContainerObject &container);
	virtual ~DrawableGridGeometry();

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
};

