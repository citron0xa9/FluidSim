#pragma once

#include "DrawableObject.h"
#include "../simulation/Vorton.h"
#include "TriangleNetObject.h"
#include "../simulation/VortonOctHeap.h"

#include <functional>
#include <memory>
#include <shared_mutex>

class VortonRendererOctHeap : public DrawableObject
{
public:
using oct_heap_getter_t = std::function<const std::unique_ptr<VortonOctHeap>&(void)>;

	VortonRendererOctHeap(Scene& scene, Program& phongProgram, const oct_heap_getter_t& octHeapGetter,
		std::shared_mutex& octHeapMutex);

	virtual ~VortonRendererOctHeap();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

	void renderedLevel(size_t level);

private:
    oct_heap_getter_t m_OctHeapGetter;
    std::shared_mutex& m_OctHeapMutex;
	TriangleNetObject m_DrawPrototype;

	size_t m_RenderedLevel;
};

