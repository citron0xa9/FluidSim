#pragma once

#include <float.h>
#include <glm/vec3.hpp>

class UniformGridGeometry
{
public:
	UniformGridGeometry(size_t numElements, const glm::vec3 &minCorner, const glm::vec3 &maxCorner);
	virtual ~UniformGridGeometry();

	glm::vec3 minCorner() const { return m_MinCorner; }
	glm::vec3 gridExtent() const { return m_GridExtent; }
	size_t gridPointCapacity() { return (pointsAmount().x * pointsAmount().y * pointsAmount().z); }
	glm::vec3 cellsPerExtent() const { return m_CellsPerExtent; }
	glm::uvec3 pointsAmount() const { return m_PointsAmount; }
	glm::uvec3 cellsAmount() const { return (pointsAmount() - glm::uvec3(1)); }
	glm::vec3 cellExtent() const { return m_CellExtent; }

private:
	void defineShape(size_t numElements, const glm::vec3 &minCorner, const glm::vec3 &maxCorner);
	void calculatePointsAmount(size_t numElements);
	void calculateGridExtent(const glm::vec3 &minCorner, const glm::vec3 &maxCorner);
	void calculateSpacing();

	glm::vec3 m_MinCorner;
	glm::vec3 m_GridExtent;	// Size of grid
	glm::vec3 m_CellExtent;   // Size of a cell
	glm::vec3 m_CellsPerExtent;
	glm::uvec3 m_PointsAmount;
};