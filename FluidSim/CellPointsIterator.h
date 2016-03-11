#pragma once
#if 0

#include <iterator>
#include <glm/vec3.hpp>
#include "UniformGrid.h"

template <class ItemT>
class CellPointsIterator : public std::iterator<std::forward_iterator_tag, ItemT>
{
public:
	CellPointsIterator(UniformGrid &grid, const size_t &startOffset);
	//CellPointsIterator(const CellPointsIterator &it);

	CellPointsIterator operator+(int inc);
	CellPointsIterator& operator++() { m_PointNumber++; return *this; }
	CellPointsIterator operator++(int);
	bool operator==(const CellPointsIterator &other);
	bool operator!=(const CellPointsIterator &other) { return (!operator==(other)); }

private:
	UniformGrid &m_BaseGrid;
	size_t m_Offsets[8];
	unsigned char m_PointNumber;
};

template<class ItemT>
inline CellPointsIterator<ItemT>::CellPointsIterator(UniformGrid & grid, const size_t & startOffset) : m_BaseGrid{ grid }, m_PointNumber{ 0 }
{
	glm::uvec3 gridPointsAmount = m_BaseGrid.GetPointsAmount();
	size_t xyPointsAmount = gridPointsAmount.x * gridPointsAmount.y;

	m_Offsets{
		startOffset,
		startOffset + 1,
		startOffset + gridPointsAmount.x,
		startOffset + gridPointsAmount.x + 1,
		startOffset + xyPointsAmount,
		startOffset + xyPointsAmount + 1,
		startOffset + xyPointsAmount + gridPointsAmount.x,
		startOffset + xyPointsAmount + gridPointsAmount.x + 1
	};
}

template<class ItemT>
inline CellPointsIterator CellPointsIterator<ItemT>::operator+(int inc)
{
	auto it = CellPointsIterator(*this);
	it.m_PointNumber += inc;
	return it;
}

template<class ItemT>
inline CellPointsIterator CellPointsIterator<ItemT>::operator++(int)
{
	auto it = CellPointsIterator(*this);
	operator++();
	return it;
}

template<class ItemT>
inline bool CellPointsIterator<ItemT>::operator==(const CellPointsIterator & other)
{
	return other.m_BaseGrid == m_BaseGrid && other.m_Offsets == m_Offsets && other.m_PointNumber ==
}
#endif