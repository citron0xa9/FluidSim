
#include "VortonOctHeap.h"
#include "VortonOctHeapElement.h"
#include "fsmath.h"

const double VortonOctHeap::m_MAX_VOLUME = 1.0;
const size_t VortonOctHeap::m_DEFAULT_DIVISIONS_COUNT = 4;

VortonOctHeap::VortonOctHeap(std::vector<Vorton>& vortons)
	: m_NullSupervorton()
{
	m_NullSupervorton.radius(vortons.back().radius());
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

VortonOctHeapElement & VortonOctHeap::root()
{
	assert(!m_Heap.empty());
	return m_Heap[0];
}

std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> VortonOctHeap::leafs()
{
	assert(!m_Heap.empty());
	std::vector<VortonOctHeapElement>::iterator begin = m_Heap.begin() + calculateFirstLeafIndex();
	return std::make_pair(begin, m_Heap.end());
}

std::pair<std::vector<VortonOctHeapElement>::iterator, std::vector<VortonOctHeapElement>::iterator> VortonOctHeap::allAtLevel(size_t level)
{
	assert(!m_Heap.empty());
	std::vector<VortonOctHeapElement>::iterator begin = m_Heap.begin() + calculateFirstIndexForLevel(level);
	std::vector<VortonOctHeapElement>::iterator end = m_Heap.begin() + calculateLastIndexForLevel(level) + 1;
	return std::make_pair(begin, end);
}

VortonOctHeapElement & VortonOctHeap::atIndex(size_t index)
{
	assert(index < m_Heap.size());
	return m_Heap[index];
}

VortonOctHeapElement & VortonOctHeap::leaftAtPosition(const glm::dvec3 & position)
{
	return atIndex(leafIndexForPosition(position));
}

glm::dvec3 VortonOctHeap::minCorner() const
{
	return m_MinCorner;
}

glm::dvec3 VortonOctHeap::extent() const
{
	return m_Extent;
}

size_t VortonOctHeap::divisions() const
{
	return m_Divisions;
}

void VortonOctHeap::calculateBoundingBox(const std::vector<Vorton>& vortons)
{
	glm::dvec3 minCorner = vortons[0].position();
	glm::dvec3 maxCorner = vortons[0].position();
	for (auto& vorton : vortons) {
		minCorner = fsmath::allMin(minCorner, vorton.position());
		maxCorner = fsmath::allMax(maxCorner, vorton.position());
	}
	m_Extent = (maxCorner - minCorner);
	glm::dvec3 adjustment = static_cast<double>(FLT_EPSILON) * m_Extent;
	minCorner -= adjustment;
	maxCorner += adjustment;
	m_Extent = (maxCorner - minCorner);
	m_MinCorner = minCorner;
	//m_Extent = fsmath::allNextPowerOf2(m_Extent);
}

void VortonOctHeap::subdivide(double maxVolume)
{
	double volume = m_Extent.x * m_Extent.y * m_Extent.z;
	size_t divisions = static_cast<size_t>(std::ceil(std::log2(volume/maxVolume) / std::log2(8)));
	
	subdivide(divisions);
}

void VortonOctHeap::subdivide(size_t divisions)
{
	m_Divisions = divisions;
	m_ExtentPerLeaf = glm::dvec3(m_Extent) / static_cast<double>(1 << m_Divisions);
	m_LeafsPerDimension = static_cast<size_t>(1) << (divisions);	//2^divisions
}

void VortonOctHeap::createEmptyOctHeap()
{
	//reserve needed heap size to prevent reallocations
	size_t heapSize = calculateNeededHeapSize();
	m_Heap.reserve(heapSize);

	//create empty heap
	size_t currentElementDepth = 0;
	size_t currentElementDepthStartOffset = 0;
	glm::dvec3 currentElementExtent = m_Extent;
	glm::dvec3 currentElementCenter = m_MinCorner + (m_Extent / 2.0);
	size_t elementsPerDimension = 1;
	size_t elementsPerDimensionSquared = 1;
	for (size_t i = 0; i < heapSize; i++) {
		if (i >= (currentElementDepthStartOffset + (static_cast<size_t>(1) << (currentElementDepth * 3)))) {	//(1 << (currentElementDepth * 3)) = 8^depth = element count in current depth layer
			//new depth reached
			currentElementDepthStartOffset = i;
			currentElementDepth++;
			currentElementExtent /= 2;
			currentElementCenter = m_MinCorner + (currentElementExtent / 2.0);
			elementsPerDimension = (static_cast<size_t>(1) << currentElementDepth);
			elementsPerDimensionSquared = elementsPerDimension * elementsPerDimension;
		}
		m_NullSupervorton.position(currentElementCenter);
		m_Heap.push_back(VortonOctHeapElement(i, m_NullSupervorton, *this, currentElementExtent));

		//increase currentElementMinCorner
		size_t currentElementCountInLayer = (i - currentElementDepthStartOffset) + 1;
		if (currentElementCountInLayer % elementsPerDimension) {
			//has to increase x axis
			currentElementCenter.x += currentElementExtent.x;
		}
		else if (currentElementCountInLayer % elementsPerDimensionSquared) {
			currentElementCenter.x = m_MinCorner.x + (currentElementExtent.x / 2.0);
			currentElementCenter.y += currentElementExtent.y;
		}
		else {
			currentElementCenter.x = m_MinCorner.x + (currentElementExtent.x / 2.0);
			currentElementCenter.y = m_MinCorner.y + (currentElementExtent.y / 2.0);
			currentElementCenter.z += currentElementExtent.z;
		}
	}
}

size_t VortonOctHeap::calculateNeededHeapSize()
{
	size_t heapSize = 0;
	for (int exponent = 0; exponent <= m_Divisions; exponent++) {
		heapSize += static_cast<size_t>(std::pow(8, exponent));
	}

	return heapSize;
}

void VortonOctHeap::initializeLeafs(std::vector<Vorton>& vortons)
{
	for (auto& vorton : vortons) {
		VortonOctHeapElement &responsibleLeaf = leaftAtPosition(vorton.position());
		responsibleLeaf.supervorton().addContainedVorton(vorton);
	}
}

void VortonOctHeap::initializeParents()
{
	assert(!m_Heap.empty());
	size_t childrenAmount = m_Heap.size() - calculateFirstLeafIndex();
	for (auto it = m_Heap.rbegin() + childrenAmount; it != m_Heap.rend(); it++) {
		it->calculateSupervortonFromChildren();
	}
}

size_t VortonOctHeap::leafIndexForPosition(const glm::dvec3 & position)
{
	if (!isInsideBoundingBox(position)) {
		std::runtime_error("VortonOctHeap::getLeafIndexForPostion: position outside of bounding box");
	}

	glm::uvec3 indices = (position - m_MinCorner) / m_ExtentPerLeaf;
	return indexForIndices(indices);
}

size_t VortonOctHeap::indexForIndices(const glm::uvec3 & indices)
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
	return calculateFirstLeafIndex() + index;
}

glm::uvec3 VortonOctHeap::indicesForIndex(size_t index)
{
	size_t firstLeafIndex = calculateFirstLeafIndex();
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

size_t VortonOctHeap::calculateFirstLeafIndex() const
{
	return static_cast<size_t>(std::floor((m_Heap.size() - 1) / 8));
}

size_t VortonOctHeap::calculateFirstIndexForLevel(size_t level) const
{
	level = std::min(level, m_Divisions);
	size_t index = 0;
	for (int i = 0; i < level; i++) {
		index += 1 << (i*3);	//8^i
	}
	assert(index < m_Heap.size());
	return index;
}

size_t VortonOctHeap::calculateLastIndexForLevel(size_t level) const
{
	level = std::min(level, m_Divisions);
	size_t index = 0;
	for (int i = 0; i < level+1; i++) {
		index += 1 << (i * 3);	//8^i
	}
	index--;
	assert(index < m_Heap.size());
	return index;
}


