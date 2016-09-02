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
	
private:
	void applyGridResolutionFactor();
	void render(const glm::mat4x4 &viewProjectTransform, const glm::dvec3 &pointPosition, size_t pointOffset, const UniformGrid<glm::dvec3> &velocityGrid);

	TriangleNetObject m_DrawPrototype;

	std::function<const std::shared_ptr<UniformGrid<glm::dvec3>>(void)> m_VelocityGridGetter;

	static const double m_MAX_VELOCITY;
	static const glm::dvec3 m_INITIAL_DIRECTION;

	std::vector<Arrow> m_CurrentArrows;
	float m_GridResolutionFactor;
	bool m_GridResolutionChanged;
};

