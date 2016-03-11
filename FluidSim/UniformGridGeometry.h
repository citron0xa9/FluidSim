#pragma once

#include <float.h>
#include <glm/vec3.hpp>

class UniformGridGeometry
{
public:
	//UniformGridGeometry();
	UniformGridGeometry(size_t numElements, const glm::vec3 &minCorner, const glm::vec3 &maxCorner, bool bPowerOf2);
	virtual ~UniformGridGeometry();

	const glm::vec3& GetMinCorner() const { return m_MinCorner; }
	const glm::vec3& GetGridExtent() const { return m_GridExtent; }
	size_t GetGridPointCapacity() { return (GetPointsAmount().x * GetPointsAmount().y * GetPointsAmount().z); }
	const glm::vec3& GetCellsPerExtent() const { return m_CellsPerExtent; }
	const glm::uvec3& GetPointsAmount() const { return m_PointsAmount; }
	const glm::uvec3& GetCellsAmount() const { return (GetPointsAmount() - glm::uvec3(1)); }
	const glm::vec3& GetCellExtent() const { return m_CellExtent; }

private:
	void DefineShape(size_t numElements, const glm::vec3 &minCorner, const glm::vec3 &maxCorner, bool bPowerOf2);
	void CalculatePointsAmount(size_t numElements, bool bPowerOf2);
	void CalculateGridExtent(const glm::vec3 &minCorner, const glm::vec3 &maxCorner);
	void CalculateSpacing();

	glm::vec3 m_MinCorner;
	glm::vec3 m_GridExtent;	// Size of grid
	glm::vec3 m_CellExtent;   // Size of a cell
	glm::vec3 m_CellsPerExtent;
	glm::uvec3 m_PointsAmount;
};