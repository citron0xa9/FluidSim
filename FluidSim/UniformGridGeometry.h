#pragma once

#include <float.h>
#include <glm/vec3.hpp>

class UniformGridGeometry
{
public:
	UniformGridGeometry(size_t numElements, const glm::dvec3 &minCorner, const glm::dvec3 &maxCorner);
	virtual ~UniformGridGeometry();

	glm::dvec3 minCorner() const { return m_MinCorner; }
	glm::dvec3 gridExtent() const { return m_GridExtent; }
	size_t gridPointCapacity() const { return (pointsAmount().x * pointsAmount().y * pointsAmount().z); }
	glm::dvec3 cellsPerExtent() const { return m_CellsPerExtent; }
	glm::uvec3 pointsAmount() const { return m_PointsAmount; }
	glm::uvec3 cellsAmount() const { return (pointsAmount() - glm::uvec3(1)); }
	glm::dvec3 cellExtent() const { return m_CellExtent; }

private:
	void defineShape(size_t numElements, const glm::dvec3 &minCorner, const glm::dvec3 &maxCorner);
	void calculatePointsAmount(size_t numElements);
	void calculateGridExtent(const glm::dvec3 &minCorner, const glm::dvec3 &maxCorner);
	void calculateSpacing();

	glm::dvec3 m_MinCorner;
	glm::dvec3 m_GridExtent;	// Size of grid
	glm::dvec3 m_CellExtent;   // Size of a cell
	glm::dvec3 m_CellsPerExtent;
	glm::uvec3 m_PointsAmount;
};