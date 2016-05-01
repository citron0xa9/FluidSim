#pragma once

#include "UniformGridGeometry.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

template <class ItemT>
class UniformGrid : public UniformGridGeometry
{
public:
	UniformGrid(const UniformGrid &that) = delete;
	UniformGrid(size_t numElements, const glm::vec3 &minCorner, const glm::vec3 &maxCorner)
		: UniformGridGeometry(numElements, minCorner, maxCorner)
	{
		init();
	}

	explicit UniformGrid(const UniformGridGeometry &that) : UniformGridGeometry(that) { init(); }
	virtual ~UniformGrid() {}


	ItemT& atPosition(const glm::vec3 &position) { return privateAtPosition(position); }
	const ItemT& atPosition(const glm::vec3 &position) const { return privateAtPosition(position); }

	ItemT& atIndices(const glm::uvec3 &indices) { return privateAtIndices(indices); }
	const ItemT& atIndices(const glm::uvec3 &indices) const { return privateAtIndices(indices); }

	ItemT& atOffset(size_t offset) { return m_Points[offset]; }
	const ItemT& atOffset(size_t offset) const { return m_Points[offset]; }


	std::pair<ItemT&, ItemT&> minMaxElement() { return std::minmax_element(m_Points.begin(), m_Points.end()); }

	ItemT interpolate(const glm::vec3 &position) const;

private:
	void init() { m_Points.resize(gridPointCapacity()); }

	ItemT& privateAtPosition(const glm::vec3 &position);
	ItemT& privateAtIndices(const glm::uvec3 &indices);
	glm::uvec3 getResponsiblePointIndices(const glm::vec3 &position) const;
	size_t offsetForIndices(const glm::uvec3 &indices) const;

	std::vector<size_t> calculateCellPointsOffsets(size_t startOffset) const;

	std::vector<ItemT> m_Points;
};

#include "fsmath.h"

template<class ItemT>
inline ItemT UniformGrid<ItemT>::interpolate(const glm::vec3 & position) const
{
	if (fsmath::anyLess(position, minCorner()) || fsmath::anyLess(minCorner() + gridExtent(), position)) {
		std::runtime_error("UniformGrid::Interpolate: given position is outside of grid");
	}

	glm::uvec3 responsiblePointIndices = getResponsiblePointIndices(position);
	glm::vec3 relativePositionNormalized = (position - minCorner()) / cellExtent() - glm::vec3(responsiblePointIndices);
	glm::vec3 oneMinusRelativePositionNormalized = glm::vec3(1) - relativePositionNormalized;

	std::vector<size_t> cellPointsOffsets = calculateCellPointsOffsets(offsetForIndices(responsiblePointIndices));

	return (oneMinusRelativePositionNormalized.x* oneMinusRelativePositionNormalized.y	* oneMinusRelativePositionNormalized.z	* atOffset(cellPointsOffsets[0])
		+ relativePositionNormalized.x			* oneMinusRelativePositionNormalized.y	* oneMinusRelativePositionNormalized.z	* atOffset(cellPointsOffsets[1])
		+ oneMinusRelativePositionNormalized.x	* relativePositionNormalized.y			* oneMinusRelativePositionNormalized.z	* atOffset(cellPointsOffsets[2])
		+ relativePositionNormalized.x			* relativePositionNormalized.y			* oneMinusRelativePositionNormalized.z	* atOffset(cellPointsOffsets[3])
		+ oneMinusRelativePositionNormalized.x	* oneMinusRelativePositionNormalized.y	* relativePositionNormalized.z			* atOffset(cellPointsOffsets[4])
		+ relativePositionNormalized.x			* oneMinusRelativePositionNormalized.y	* relativePositionNormalized.z			* atOffset(cellPointsOffsets[5])
		+ oneMinusRelativePositionNormalized.x	* relativePositionNormalized.y			* relativePositionNormalized.z			* atOffset(cellPointsOffsets[6])
		+ relativePositionNormalized.x			* relativePositionNormalized.y			* relativePositionNormalized.z			* atOffset(cellPointsOffsets[7]));
}

template<class ItemT>
inline ItemT& UniformGrid<ItemT>::privateAtPosition(const glm::vec3 &position)
{
	glm::uvec3 indices = getResponsiblePointIndices(position);
	return privateAtIndices(indices);
}

template<class ItemT>
inline ItemT& UniformGrid<ItemT>::privateAtIndices(const glm::uvec3 &indices)
{
	return m_Points[offsetForIndices(indices)];
}

template<class ItemT>
inline glm::uvec3 UniformGrid<ItemT>::getResponsiblePointIndices(const glm::vec3 & position) const
{
	glm::vec3 relativePosition = position - minCorner();
	return (relativePosition * cellsPerExtent());
}

template<class ItemT>
inline size_t UniformGrid<ItemT>::offsetForIndices(const glm::uvec3 & indices) const
{
	return (indices.x + pointsAmount().x * (indices.y + pointsAmount().y * indices.z));
}

template<class ItemT>
inline std::vector<size_t> UniformGrid<ItemT>::calculateCellPointsOffsets(size_t startOffset) const
{
	size_t xyPointsAmount = pointsAmount().x * pointsAmount().y;

	return {
		startOffset,
		startOffset + 1,
		startOffset + pointsAmount().x,
		startOffset + pointsAmount().x + 1,
		startOffset + xyPointsAmount,
		startOffset + xyPointsAmount + 1,
		startOffset + xyPointsAmount + pointsAmount().x,
		startOffset + xyPointsAmount + pointsAmount().x + 1
	};
}
