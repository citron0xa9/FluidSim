
#include "UniformGridGeometry.h"
#include "../util/fsmath.h"
#include <algorithm>

//UniformGridGeometry::UniformGridGeometry()
//{
//}

UniformGridGeometry::UniformGridGeometry(size_t numElements, const glm::dvec3 & minCorner, const glm::dvec3 & maxCorner)
{
	defineShape(std::max(static_cast<size_t>(1u), numElements), minCorner, maxCorner);
}


UniformGridGeometry::~UniformGridGeometry()
{
}

void UniformGridGeometry::defineShape(size_t numElements, const glm::dvec3 & minCorner, const glm::dvec3 & maxCorner)
{
	m_MinCorner = minCorner;
	calculateGridExtent(minCorner, maxCorner);

	calculatePointsAmount(numElements);

	//compute final spacing using the calculated number of points
	calculateSpacing();
}

void UniformGridGeometry::calculateSpacing()
{
	glm::uvec3 cellAmount = cellsAmount();
	m_CellExtent = gridExtent() / glm::dvec3(cellAmount);
	m_CellsPerExtent = glm::dvec3(1) / m_CellExtent;
}

void UniformGridGeometry::calculatePointsAmount(size_t numElements)
{
	const double volume = m_GridExtent.x * m_GridExtent.y * m_GridExtent.z;

	const double cellLength = std::cbrt(volume / numElements);  //first calculate temporary uniform length, cbrt = cubic root

	glm::uvec3 numCells = glm::uvec3(
		std::ceil(m_GridExtent.x / cellLength),
		std::ceil(m_GridExtent.y / cellLength),
		std::ceil(m_GridExtent.z / cellLength));

	//adjust number of cells
	while (numCells.x * numCells.y * numCells.z >= numElements * 8)
	{   // Grid capacity is excessive.
		// This can occur when the trial numCells is below 0.5 in which case the integer arithmetic loses the subtlety.
        // Also the dimensions might be sized very differently
		numCells.x = std::max(1u, numCells.x / 2);
		numCells.y = std::max(1u, numCells.y / 2);
		numCells.z = std::max(1u, numCells.z / 2);
	}

	m_PointsAmount = numCells + glm::uvec3(1);
}

void UniformGridGeometry::calculateGridExtent(const glm::dvec3 & minCorner, const glm::dvec3 & maxCorner)
{
	static const double NUDGE = 1.0 + FLT_EPSILON;  // slightly expand size to ensure robust containment even with roundoff
	glm::dvec3 extent = (maxCorner - minCorner);
	m_GridExtent = extent * NUDGE;
}
