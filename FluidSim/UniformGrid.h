#pragma once

#include "UniformGridGeometry.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

template <class ItemT>
class UniformGrid : public UniformGridGeometry
{
public:
	//UniformGrid() {}
	UniformGrid(size_t numElements, const glm::vec3 &minCorner, const glm::vec3 &maxCorner, bool bPowerOf2)
		: UniformGridGeometry(numElements, minCorner, maxCorner, bPowerOf2)
	{
		Init();
	}

	explicit UniformGrid(const UniformGridGeometry &that) : UniformGridGeometry(that) { Init(); }
	virtual ~UniformGrid() {}


	ItemT& AtPosition(const glm::vec3 &position) { return PrivateAtPosition(position); }
	const ItemT& AtPosition(const glm::vec3 &position) const { return PrivateAtPosition(position); }

	ItemT& AtIndices(const glm::uvec3 &indices) { return PrivateAtIndices(indices); }
	const ItemT& AtIndices(const glm::uvec3 &indices) const { return PrivateAtIndices(indices); }

	ItemT& AtOffset(size_t offset) { return m_Points[offset]; }
	const ItemT& AtOffset(size_t offset) const { return m_Points[offset]; }


	std::pair<ItemT&, ItemT&> MinMaxElement() { return std::minmax_element(m_Points.begin(), m_Points.end()); }

	ItemT Interpolate(const glm::vec3 &position) const;

private:
	//Deny copy construction
	UniformGrid(const UniformGrid &that) : UniformGridGeometry(that) {}

	void Init() { m_Points.resize(GetGridPointCapacity()); }

	ItemT& PrivateAtPosition(const glm::vec3 &position);
	ItemT& PrivateAtIndices(const glm::uvec3 &indices);
	glm::uvec3 GetResponsiblePointIndices(const glm::vec3 &position) const;
	size_t OffsetForIndices(const glm::uvec3 &indices) const;

	std::vector<size_t> CellPointsOffsets(size_t startOffset) const;

	std::vector<ItemT> m_Points;
};

#include "fsmath.h"

template<class ItemT>
inline ItemT UniformGrid<ItemT>::Interpolate(const glm::vec3 & position) const
{
	if (fsmath::AnyLess(position, GetMinCorner()) || fsmath::AnyLess(GetMinCorner() + GetGridExtent(), position)) {
		std::runtime_error("UniformGrid::Interpolate: given position is outside of grid");
	}

	glm::uvec3 responsiblePointIndices = GetResponsiblePointIndices(position);
	glm::vec3 relativePositionNormalized = (position - GetMinCorner()) / GetCellExtent() - glm::vec3(responsiblePointIndices);
	glm::vec3 oneMinusRelativePositionNormalized = glm::vec3(1) - relativePositionNormalized;

	std::vector<size_t> cellPointsOffsets = CellPointsOffsets(OffsetForIndices(responsiblePointIndices));

	return (oneMinusRelativePositionNormalized.x* oneMinusRelativePositionNormalized.y	* oneMinusRelativePositionNormalized.z	* AtOffset(cellPointsOffsets[0])
		+ relativePositionNormalized.x			* oneMinusRelativePositionNormalized.y	* oneMinusRelativePositionNormalized.z	* AtOffset(cellPointsOffsets[1])
		+ oneMinusRelativePositionNormalized.x	* relativePositionNormalized.y			* oneMinusRelativePositionNormalized.z	* AtOffset(cellPointsOffsets[2])
		+ relativePositionNormalized.x			* relativePositionNormalized.y			* oneMinusRelativePositionNormalized.z	* AtOffset(cellPointsOffsets[3])
		+ oneMinusRelativePositionNormalized.x	* oneMinusRelativePositionNormalized.y	* relativePositionNormalized.z			* AtOffset(cellPointsOffsets[4])
		+ relativePositionNormalized.x			* oneMinusRelativePositionNormalized.y	* relativePositionNormalized.z			* AtOffset(cellPointsOffsets[5])
		+ oneMinusRelativePositionNormalized.x	* relativePositionNormalized.y			* relativePositionNormalized.z			* AtOffset(cellPointsOffsets[6])
		+ relativePositionNormalized.x			* relativePositionNormalized.y			* relativePositionNormalized.z			* AtOffset(cellPointsOffsets[7]));
}

template<class ItemT>
inline ItemT& UniformGrid<ItemT>::PrivateAtPosition(const glm::vec3 &position)
{
	glm::uvec3 indices = GetResponsiblePointIndices(position);
	return PrivateAtIndices(indices);
}

template<class ItemT>
inline ItemT& UniformGrid<ItemT>::PrivateAtIndices(const glm::uvec3 &indices)
{
	return m_Points[OffsetForIndices(indices)];
}

template<class ItemT>
inline glm::uvec3 UniformGrid<ItemT>::GetResponsiblePointIndices(const glm::vec3 & position) const
{
	glm::vec3 relativePosition = position - GetMinCorner();
	return (relativePosition * GetCellsPerExtent());
}

template<class ItemT>
inline size_t UniformGrid<ItemT>::OffsetForIndices(const glm::uvec3 & indices) const
{
	return (indices.x + GetPointsAmount().x * (indices.y + GetPointsAmount().y * indices.z));
}

template<class ItemT>
inline std::vector<size_t> UniformGrid<ItemT>::CellPointsOffsets(size_t startOffset) const
{
	size_t xyPointsAmount = GetPointsAmount().x * GetPointsAmount().y;

	return {
		startOffset,
		startOffset + 1,
		startOffset + GetPointsAmount().x,
		startOffset + GetPointsAmount().x + 1,
		startOffset + xyPointsAmount,
		startOffset + xyPointsAmount + 1,
		startOffset + xyPointsAmount + GetPointsAmount().x,
		startOffset + xyPointsAmount + GetPointsAmount().x + 1
	};
}
