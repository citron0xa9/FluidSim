#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "Supervorton.h"

class VortonOctHeap
{
	friend class VortonOctHeapElement;
public:
	VortonOctHeap(const std::vector<Vorton> &vortons);
	~VortonOctHeap();

	VortonOctHeapElement& getRoot();
	std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> getLeafs();
	VortonOctHeapElement& atIndex(size_t index);
	VortonOctHeapElement& leaftAtPosition(const glm::vec3 &position);

private:
	void calculateBoundingBox(const std::vector<Vorton> &vortons);
	void subdivide(float maxVolume);
	void createEmptyOctHeap();
	size_t calculateNeededHeapSize();
	void initializeLeafs(const std::vector<Vorton> &vortons);
	void initializeParents();

	size_t getLeafIndexForPosition(const glm::vec3 &position);

	bool isInsideBoundingBox(const glm::vec3 &position);
	
	glm::vec3 m_MinCorner;
	glm::uvec3 m_Extent;
	size_t m_Divisions;
	glm::vec3 m_ExtentPerLeaf;

	static const float m_MAX_VOLUME;

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