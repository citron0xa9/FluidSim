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

	VortonOctHeapElement& getRoot();
	std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> getLeafs();
	VortonOctHeapElement& atIndex(size_t index);
	VortonOctHeapElement& leaftAtPosition(const glm::vec3 &position);

	glm::vec3 getMinCorner() const;
	glm::vec3 getExtent() const;

private:
	void calculateBoundingBox(const std::vector<Vorton> &vortons);
	void subdivide(float maxVolume);
	void subdivide(size_t divisions);
	void createEmptyOctHeap();
	size_t calculateNeededHeapSize();
	void initializeLeafs(std::vector<Vorton> &vortons);
	void initializeParents();

	size_t getLeafIndexForPosition(const glm::vec3 &position);
	size_t getIndexForIndices(const glm::uvec3 &indices);
	glm::uvec3 getIndicesForIndex(size_t index);

	size_t getFirstLeafIndex() const;

	bool isInsideBoundingBox(const glm::vec3 &position);
	
	glm::vec3 m_MinCorner;
	glm::vec3 m_Extent;
	size_t m_Divisions;
	size_t m_LeafsPerDimension;
	glm::vec3 m_ExtentPerLeaf;

	static const float m_MAX_VOLUME;
	static const size_t m_DEFAULT_DIVISIONS_COUNT;

	std::vector<VortonOctHeapElement> m_Heap;
	Supervorton m_NullSupervorton;
};

inline bool VortonOctHeap::isInsideBoundingBox(const glm::vec3 & position)
{
	return (
		(!fsmath::AnyLess(position, m_MinCorner))
		&& (!fsmath::AnyLess(m_MinCorner + glm::vec3(m_Extent), position))
		);
}