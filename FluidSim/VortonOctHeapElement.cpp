
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

Supervorton & VortonOctHeapElement::getSupervorton()
{
	return m_Supervorton;
}

std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> VortonOctHeapElement::getChildren()
{
	size_t startIndex = m_HeapIndex * 8 + 1;
	size_t endIndex = m_HeapIndex * 8 + 8;
	if (endIndex > (m_Owner.m_Heap.size() - 1)) {
		endIndex = m_Owner.m_Heap.size() - 1;
	}
	return std::make_pair(m_Owner.m_Heap.begin() + startIndex, m_Owner.m_Heap.begin() + endIndex);
}

void VortonOctHeapElement::calculateSupervortonFromChildren()
{
	auto childrenInterval = getChildren();
	std::vector<Vorton*> childrenVortons;
	for (auto it = childrenInterval.first; it != childrenInterval.second; it++) {
		childrenVortons.push_back(&(it->getSupervorton()));
	}
	m_Supervorton.setContainedVortonPtrs(childrenVortons);
}

bool VortonOctHeapElement::hasChildren()
{
	auto childrenIterators = getChildren();
	return !(childrenIterators.first == childrenIterators.second);
}

glm::vec3 VortonOctHeapElement::calculateVelocity(const glm::vec3 & position)
{
	if (fsmath::insideBox(position, m_Supervorton.getPosition(), m_Extent)) {
		return calculateVelocityAccurate(position);
	}
	else {
		return calculateVelocityFast(position);
	}
}

std::vector<VortonOctHeapElement*> VortonOctHeapElement::getForwardNeighbors()
{
	std::vector<VortonOctHeapElement*> forwardNeighbors;
	glm::uvec3 ownIndices = m_Owner.getIndicesForIndex(m_HeapIndex);
	for (int i = 0; i < 3; i++) {
		if (ownIndices[i] < (m_Owner.m_LeafsPerDimension - 2)) {
			glm::uvec3 neighborIndicesOffset(0);
			neighborIndicesOffset[i] = 1;
			size_t neighborIndex = m_Owner.getIndexForIndices(ownIndices + neighborIndicesOffset);
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
		if (m_Supervorton.getContainedVortonPtrs().empty()) {
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
	auto childrenIterators = getChildren();
	glm::vec3 velocity(0);
	for (; childrenIterators.first != childrenIterators.second; childrenIterators.first++) {
		velocity += childrenIterators.first->calculateVelocity(position);
	}
	return velocity;
}

glm::vec3 VortonOctHeapElement::calculateVelocityViaContainedVortons(const glm::vec3 & position)
{
	glm::vec3 velocity(0);
	for (const Vorton *vortonPtr : m_Supervorton.getContainedVortonPtrs()) {
		velocity += vortonPtr->inducedVelocity(position);
	}
	return velocity;
}
