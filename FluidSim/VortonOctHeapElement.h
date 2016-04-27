#pragma once

#include "Supervorton.h"
#include "VortonOctHeap.h"
#include <vector>

class VortonOctHeapElement
{
public:
	VortonOctHeapElement(size_t index, const Supervorton &supervorton, VortonOctHeap &owner, const glm::vec3 &extent);
	~VortonOctHeapElement();

	Supervorton& supervorton();

	std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> children();
	void calculateSupervortonFromChildren();

	bool hasChildren();

	glm::vec3 calculateVelocity(const glm::vec3 &position);

	std::vector<VortonOctHeapElement*> forwardNeighbors();

private:
	glm::vec3 calculateVelocityAccurate(const glm::vec3 &position);
	glm::vec3 calculateVelocityFast(const glm::vec3 &position);
	glm::vec3 calculateVelocityViaChildren(const glm::vec3 &position);
	glm::vec3 calculateVelocityViaContainedVortons(const glm::vec3 &position);

	VortonOctHeap &m_Owner;
	size_t m_HeapIndex;

	glm::vec3 m_Extent;

	Supervorton m_Supervorton;
};

