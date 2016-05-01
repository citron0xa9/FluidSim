
#include "VortonOctHeapElement.h"

VortonOctHeapElement::VortonOctHeapElement(size_t index, const Supervorton & supervorton, VortonOctHeap &owner, const glm::vec3 &extent)
	: m_HeapIndex{index}, m_Supervorton{supervorton}, m_Owner{owner}, m_Extent{extent}
{
}


VortonOctHeapElement::~VortonOctHeapElement()
{
}

/*void VortonOctHeapElement::setSupervorton(const Supervorton & supervorton)
{
	m_Supervorton = supervorton;
}*/

Supervorton & VortonOctHeapElement::supervorton()
{
	return m_Supervorton;
}

std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> VortonOctHeapElement::children()
{
	size_t startIndex = m_HeapIndex * 8 + 1;
	size_t endIndex = m_HeapIndex * 8 + 9;
	if (endIndex > (m_Owner.m_Heap.size())) {
		endIndex = m_Owner.m_Heap.size();
	}
	if (startIndex > endIndex) {
		startIndex = endIndex;
	}
	return std::make_pair(m_Owner.m_Heap.begin() + startIndex, m_Owner.m_Heap.begin() + endIndex);
}

void VortonOctHeapElement::calculateSupervortonFromChildren()
{
	auto childrenInterval = children();
	std::vector<Vorton*> childrenVortons;
	for (auto it = childrenInterval.first; it != childrenInterval.second; it++) {
		childrenVortons.push_back(&(it->supervorton()));
	}
	m_Supervorton.containedVortonPtrs(childrenVortons);
}

bool VortonOctHeapElement::hasChildren()
{
	return (((m_HeapIndex*8)+1) < m_Owner.m_Heap.size());
}

glm::vec3 VortonOctHeapElement::calculateVelocity(const glm::vec3 & position)
{
	if (fsmath::insideBox(position, m_Supervorton.position() - (m_Extent / 2.0f), m_Extent)) {
		return calculateVelocityAccurate(position);
	}
	else {
		return calculateVelocityFast(position);
	}
}

std::vector<VortonOctHeapElement*> VortonOctHeapElement::forwardNeighbors()
{
	std::vector<VortonOctHeapElement*> forwardNeighbors;
	glm::uvec3 ownIndices = m_Owner.indicesForIndex(m_HeapIndex);
	for (int i = 0; i < 3; i++) {
		if (ownIndices[i] < (m_Owner.m_LeafsPerDimension - 2)) {
			glm::uvec3 neighborIndicesOffset(0);
			neighborIndicesOffset[i] = 1;
			size_t neighborIndex = m_Owner.indexForIndices(ownIndices + neighborIndicesOffset);
			forwardNeighbors.push_back(&(m_Owner.atIndex(neighborIndex)));
		}
	}

	return forwardNeighbors;
}

glm::vec3 VortonOctHeapElement::calculateVelocityAccurate(const glm::vec3 & position)
{
	if (hasChildren()) {
		return calculateVelocityViaChildren(position);
	}
	else {
		if (m_Supervorton.containedVortonPtrs().empty()) {
			//no real vortons represented -> vorticity 0 -> doesnt induce velocity
			return glm::vec3(0);
		}
		else {
			return calculateVelocityViaContainedVortons(position);
		}
	}
}

glm::vec3 VortonOctHeapElement::calculateVelocityFast(const glm::vec3 & position)
{
	return m_Supervorton.inducedVelocity(position);
}

glm::vec3 VortonOctHeapElement::calculateVelocityViaChildren(const glm::vec3 & position)
{
	glm::vec3 velocity(0);
	for (auto childrenIterators = children(); childrenIterators.first != childrenIterators.second; childrenIterators.first++) {
		velocity += childrenIterators.first->calculateVelocity(position);
	}
	return velocity;
}

glm::vec3 VortonOctHeapElement::calculateVelocityViaContainedVortons(const glm::vec3 & position)
{
	glm::vec3 velocity(0);
	for (const Vorton *vortonPtr : m_Supervorton.containedVortonPtrs()) {
		velocity += vortonPtr->inducedVelocity(position);
	}
	return velocity;
}
