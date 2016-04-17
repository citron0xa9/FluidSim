
#include "UniformGridGeometry.h"
#include "fsmath.h"
#include <algorithm>

//UniformGridGeometry::UniformGridGeometry()
//{
//}

UniformGridGeometry::UniformGridGeometry(size_t numElements, const glm::vec3 & minCorner, const glm::vec3 & maxCorner, bool bPowerOf2)
{
	DefineShape(numElements, minCorner, maxCorner, bPowerOf2);
}


UniformGridGeometry::~UniformGridGeometry()
{
}

void UniformGridGeometry::DefineShape(size_t numElements, const glm::vec3 & minCorner, const glm::vec3 & maxCorner, bool bPowerOf2)
{
	m_MinCorner = minCorner;
	CalculateGridExtent(minCorner, maxCorner);

	CalculatePointsAmount(numElements, bPowerOf2);

	//compute final spacing using the calculated number of points
	CalculateSpacing();
}

void UniformGridGeometry::CalculateSpacing()
{
	glm::uvec3 cellAmount = GetCellsAmount();
	m_CellExtent = GetGridExtent() / glm::vec3(cellAmount);
	m_CellsPerExtent = glm::vec3(1) / m_CellExtent;
}

void UniformGridGeometry::CalculatePointsAmount(size_t numElements, bool bPowerOf2)
{
	const float volume = m_GridExtent.x * m_GridExtent.y * m_GridExtent.z;
	const float cellLength = std::cbrt(volume / numElements);  //first calculate temporary uniform length, cbrt = cubic root

	glm::uvec3 numCells = glm::uvec3(
		std::ceil(m_GridExtent.x / cellLength),
		std::ceil(m_GridExtent.y / cellLength),
		std::ceil(m_GridExtent.z / cellLength));

	//adjust number of cells
	if (bPowerOf2)
	{   // Choose number of gridcells to be powers of 2.
		// This will simplify subdivision in a NestedGrid.
		numCells.x = fsmath::NextPowerOf2(numCells.x);
		numCells.y = fsmath::NextPowerOf2(numCells.y);
		numCells.z = fsmath::NextPowerOf2(numCells.z);
	}
	while (numCells.x * numCells.y * numCells.z >= numElements * 8)
	{   // Grid capacity is excessive.
		// This can occur when the trial numCells is below 0.5 in which case the integer arithmetic loses the subtlety.
		numCells.x = std::max(1u, numCells.x / 2);
		numCells.y = std::max(1u, numCells.y / 2);
		numCells.z = std::max(1u, numCells.z / 2);
	}

	m_PointsAmount = numCells + glm::uvec3(1);
}

void UniformGridGeometry::CalculateGridExtent(const glm::vec3 & minCorner, const glm::vec3 & maxCorner)
{
	static const float NUDGE = 1.0f + FLT_EPSILON;  // slightly expand size to ensure robust containment even with roundoff
	glm::vec3 extent = (maxCorner - minCorner);
	m_GridExtent = extent * NUDGE;
}
