#pragma once

#include "Supervorton.h"
#include "VortonOctHeap.h"

class VortonOctHeapElement
{
public:
	VortonOctHeapElement(size_t index, const Supervorton &supervorton, VortonOctHeap &owner);
	~VortonOctHeapElement();

	void setSupervorton(const Supervorton &supervorton);
	Supervorton& getSupervorton();

	std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> getChildren();
	void calculateSupervortonFromChildren();

private:
	VortonOctHeap &m_Owner;
	size_t m_HeapIndex;

	Supervorton m_Supervorton;
};

