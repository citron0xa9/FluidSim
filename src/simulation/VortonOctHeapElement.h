#pragma once

#include "Supervorton.h"
#include "VortonOctHeap.h"
#include <vector>

class VortonOctHeapElement
{
public:
	VortonOctHeapElement(size_t index, const Supervorton &supervorton, VortonOctHeap &owner, const glm::dvec3 &extent);
	~VortonOctHeapElement();

	Supervorton& supervorton();

	std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> children();
	void calculateSupervortonFromChildren();

	bool hasChildren();

	glm::dvec3 calculateVelocity(const glm::dvec3 &position);

	std::vector<VortonOctHeapElement*> forwardNeighbors();

private:
	glm::dvec3 calculateVelocityAccurate(const glm::dvec3 &position);
	glm::dvec3 calculateVelocityFast(const glm::dvec3 &position);
	glm::dvec3 calculateVelocityViaChildren(const glm::dvec3 &position);
	glm::dvec3 calculateVelocityViaContainedVortons(const glm::dvec3 &position);

	VortonOctHeap &m_Owner;
	size_t m_HeapIndex;

	glm::dvec3 m_Extent;

	Supervorton m_Supervorton;
};

