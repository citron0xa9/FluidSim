#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "DrawableObject.h"
#include "TriangleNetObject.h"
#include "UniformGrid.h"
#include "Arrow.h"

class VectorFieldRenderer : public DrawableObject
{
public:
	VectorFieldRenderer(Scene &scene, std::function<const std::shared_ptr<UniformGrid<glm::dvec3>>(void)> gridGetter, Program& phongProgram);
	virtual ~VectorFieldRenderer();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

	void gridResolutionFactor(float factor);

    void renderLines(const bool shouldRender);
    bool isRenderingLines() const;

    void renderArrows(const bool shouldRender);
    bool isRenderingArrows() const;
	
private:
    bool m_RenderArrows;
    bool m_RenderLines;

	void applyGridResolutionFactor();
	void render(const glm::mat4x4 &viewProjectTransform, const glm::dvec3 &pointPosition, size_t pointOffset, const UniformGrid<glm::dvec3> &velocityGrid,
        std::vector<std::function<void()>>& lineCreationFunctions);
    void renderArrow(const glm::mat4x4 &viewProjectTransform, const glm::dvec3 &pointPosition, size_t pointOffset, const UniformGrid<glm::dvec3> &velocityGrid);
    void renderLines(const glm::mat4x4 &viewProjectTransform);
    void createLine(const glm::dvec3 &pointPosition, size_t pointOffset, const UniformGrid<glm::dvec3> &velocityGrid);

	TriangleNetObject m_DrawPrototype;

	std::function<const std::shared_ptr<UniformGrid<glm::dvec3>>(void)> m_VelocityGridGetter;

	static const double m_MAX_VELOCITY;
	static const glm::dvec3 m_INITIAL_DIRECTION;

	std::vector<Arrow> m_CurrentArrows;
	float m_GridResolutionFactor;
	bool m_GridResolutionChanged;

    VertexBufO m_LinesVerticesBuf;
    VertexArrO m_LinesVao;
    Program m_LinesRenderProgram;

    static constexpr double m_LineStepDistance = 0.01;
    static constexpr size_t m_LineStepAmount = 1000u;

    std::vector<GLfloat> m_LineVerticesRAM;

    void setupLinesRenderProgram();
    void setupLinesVao();

    glm::vec3 rgbColorFromVelocity(const glm::dvec3& velocity) const;
    void insertLinePoint(const glm::dvec3& position, const glm::vec3& color, const size_t offset);
};

