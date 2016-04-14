
#include "VortonOctHeap.h"
#include "VortonOctHeapElement.h"
#include "fsmath.h"

const float VortonOctHeap::m_MAX_VOLUME = 1.0f;
const size_t VortonOctHeap::m_DEFAULT_DIVISIONS_COUNT = 4;

VortonOctHeap::VortonOctHeap(std::vector<Vorton>& vortons)
	: m_NullSupervorton(vortons.back())
{
	m_NullSupervorton.setVorticity(glm::vec3(0));

	if (vortons.empty()) {
		std::runtime_error("VortonOctHeap::VortonOctHeap(const std::vector<Vorton>& vortons): given vorton vector is empty");
	}
	calculateBoundingBox(vortons);
	subdivide(m_DEFAULT_DIVISIONS_COUNT);
	createEmptyOctHeap();
	initializeLeafs(vortons);
	initializeParents();
}

VortonOctHeap::~VortonOctHeap()
{
}

VortonOctHeapElement & VortonOctHeap::getRoot()
{
	assert(!m_Heap.empty());
	return m_Heap[0];
}

std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> VortonOctHeap::getLeafs()
{
	assert(!m_Heap.empty());
	size_t firstLeafIndex = std::floor((m_Heap.size() - 1) / 8) + 1;
	std::vector<VortonOctHeapElement>::iterator begin = m_Heap.begin() + firstLeafIndex;
	return std::make_pair(begin, m_Heap.end());
}

VortonOctHeapElement & VortonOctHeap::atIndex(size_t index)
{
	assert(index < m_Heap.size());
	return m_Heap[index];
}

VortonOctHeapElement & VortonOctHeap::leaftAtPosition(const glm::vec3 & position)
{
	return atIndex(getLeafIndexForPosition(position));
}

glm::vec3 VortonOctHeap::getMinCorner() const
{
	return m_MinCorner;
}

glm::vec3 VortonOctHeap::getExtent() const
{
	return m_Extent;
}

void VortonOctHeap::calculateBoundingBox(const std::vector<Vorton>& vortons)
{
	glm::vec3 minCorner = vortons[0].getPosition();
	glm::vec3 maxCorner = vortons[0].getPosition();
	for (auto& vorton : vortons) {
		minCorner = fsmath::allMin(minCorner, vorton.getPosition());
		maxCorner = fsmath::allMax(maxCorner, vorton.getPosition());
	}
	m_MinCorner = minCorner;
	m_Extent = maxCorner - minCorner;
	m_Extent = fsmath::allNextPowerOf2(m_Extent);
}

void VortonOctHeap::subdivide(float maxVolume)
{
	float volume = m_Extent.x * m_Extent.y * m_Extent.z;
	size_t divisions = std::ceil(std::log2(volume/maxVolume) / std::log2(8));
	
	subdivide(divisions);
}

void VortonOctHeap::subdivide(size_t divisions)
{
	m_Divisions = divisions;
	m_ExtentPerLeaf = glm::vec3(m_Extent) / static_cast<float>(1 << m_Divisions);
	m_LeafsPerDimension = 1 << (divisions * 3);	//8^divisions
}

void VortonOctHeap::createEmptyOctHeap()
{
	//reserve needed heap size to prevent reallocations
	size_t heapSize = calculateNeededHeapSize();
	m_Heap.reserve(heapSize);

	//create empty heap
	size_t currentElementDepth = 0;
	size_t currentElementDepthStartOffset = 0;
	glm::vec3 currentElementExtent = m_Extent;
	for (size_t i = 0; i < heapSize; i++) {
		if (i >= (currentElementDepthStartOffset + (1 << (currentElementDepth * 3)))) {	//(1 << (currentElementDepth * 3)) = 8^depth = element count in current depth layer
			//new depth reached
			currentElementDepthStartOffset = i;
			currentElementDepth++;
			currentElementExtent /= 2;
		}
		m_Heap.push_back(VortonOctHeapElement(i, m_NullSupervorton, *this, currentElementExtent));
	}
}

size_t VortonOctHeap::calculateNeededHeapSize()
{
	size_t heapSize = 0;
	for (int exponent = 0; exponent <= m_Divisions; exponent++) {
		heapSize += std::pow(8, exponent);
	}

	return heapSize;
}

void VortonOctHeap::initializeLeafs(std::vector<Vorton>& vortons)
{
	for (auto& vorton : vortons) {
		VortonOctHeapElement &responsibleLeaf = leaftAtPosition(vorton.getPosition());
		responsibleLeaf.getSupervorton().addContainedVorton(vorton);
	}
}

void VortonOctHeap::initializeParents()
{
	assert(!m_Heap.empty());
	size_t firstLeafIndex = std::floor((m_Heap.size() - 1) / 8) + 1;
	size_t childrenAmount = m_Heap.size() - firstLeafIndex;
	for (auto it = m_Heap.rbegin() + childrenAmount; it != m_Heap.rend(); it++) {
		it->calculateSupervortonFromChildren();
	}
}

size_t VortonOctHeap::getLeafIndexForPosition(const glm::vec3 & position)
{
	if (!isInsideBoundingBox(position)) {
		std::runtime_error("VortonOctHeap::getLeafIndexForPostion: position outside of bounding box");
	}

	glm::uvec3 indices = position / m_ExtentPerLeaf;
	return getIndexForIndices(indices);
}

size_t VortonOctHeap::getIndexForIndices(const glm::uvec3 & indices)
{
	//magic, no comments needed ;) TODO: add comments
	size_t index = 0;
	for (int i = 0; i < m_Divisions; i++) {
		size_t mask = 1 << i;
		for (int j = 0; j < 3; j++) {
			size_t onlyBit = indices[j] & mask;
			index |= onlyBit << (2 * i + j);
		}
	}
	size_t firstLeafIndex = std::floor((m_Heap.size() - 1) / 8) + 1;
	return firstLeafIndex + index;
}

glm::uvec3 VortonOctHeap::getIndicesForIndex(size_t index)
{
	size_t firstLeafIndex = std::floor((m_Heap.size() - 1) / 8) + 1;
	assert(index >= firstLeafIndex);
	index -= firstLeafIndex;
	//magic, no comments needed ;) TODO: add comments
	glm::uvec3 indices(0);
	for (int i = 0; i < m_Divisions; i++) {
		size_t baseMask = 1 << (i*3);
		for (int j = 0; j < 3; j++) {
			size_t currentMask = baseMask << j;
			size_t onlyBit = index & currentMask;
			indices[j] |= onlyBit >> (2 * i + j);
		}
	}
	return indices;
}


