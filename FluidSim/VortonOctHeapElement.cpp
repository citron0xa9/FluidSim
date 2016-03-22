
#include "VortonOctHeapElement.h"

VortonOctHeapElement::VortonOctHeapElement(size_t index, const Supervorton & supervorton, VortonOctHeap &owner)
	: m_HeapIndex{index}, m_Supervorton{supervorton}, m_Owner{owner}
{
}


VortonOctHeapElement::~VortonOctHeapElement()
{
}

void VortonOctHeapElement::setSupervorton(const Supervorton & supervorton)
{
	m_Supervorton = supervorton;
}

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
	std::vector<const Vorton&> childrenVortons;
	for (auto it = childrenInterval.first; it != childrenInterval.second; it++) {
		childrenVortons.push_back(it->getSupervorton());
	}
	m_Supervorton = Supervorton(childrenVortons);
}
