#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "Supervorton.h"
#include "fsmath.h"

class VortonOctHeapElement;

class VortonOctHeap
{
	friend class VortonOctHeapElement;
public:
	VortonOctHeap(std::vector<Vorton> &vortons);
	~VortonOctHeap();

	VortonOctHeapElement& root();
	std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> leafs();
	std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> allAtLevel(size_t level);
	VortonOctHeapElement& atIndex(size_t index);
	VortonOctHeapElement& leaftAtPosition(const glm::dvec3 &position);

	glm::dvec3 minCorner() const;
	glm::dvec3 extent() const;
	size_t divisions() const;

private:
	void calculateBoundingBox(const std::vector<Vorton> &vortons);
	size_t calculateNeededDivisions(const size_t numElements);
	void subdivide(double maxVolume);
	void subdivide(size_t divisions);
	void createEmptyOctHeap();
	size_t calculateNeededHeapSize();
	void initializeLeafs(std::vector<Vorton> &vortons);
	void initializeParents();

	size_t leafIndexForPosition(const glm::dvec3 &position);
	size_t indexForIndices(const glm::uvec3 &indices);
	glm::uvec3 indicesForIndex(size_t index);

	size_t calculateFirstLeafIndex() const;
	size_t calculateFirstIndexForLevel(size_t level) const;
	size_t calculateLastIndexForLevel(size_t level) const;

	bool isInsideBoundingBox(const glm::dvec3 &position);
	
	glm::dvec3 m_MinCorner;
	glm::dvec3 m_Extent;
	size_t m_Divisions;
	size_t m_LeafsPerDimension;
	glm::dvec3 m_ExtentPerLeaf;

	static const double m_MAX_VOLUME;
	static const size_t m_DEFAULT_DIVISIONS_COUNT;

	std::vector<VortonOctHeapElement> m_Heap;
	Supervorton m_NullSupervorton;
};

inline bool VortonOctHeap::isInsideBoundingBox(const glm::dvec3 & position)
{
	return (
		(!fsmath::anyLess(position, m_MinCorner))
		&& (!fsmath::anyLess(m_MinCorner + glm::dvec3(m_Extent), position))
		);
}