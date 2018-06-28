#pragma once

#include "DrawableObject.h"
#include "../simulation/Vorton.h"
#include "TriangleNetObject.h"
#include "../simulation/VortonOctHeap.h"

class VortonRendererOctHeap : public DrawableObject
{
public:
	VortonRendererOctHeap(Scene &scene, Program &phongProgram, std::function<const std::shared_ptr<VortonOctHeap>(void)> octHeapGetter);
	virtual ~VortonRendererOctHeap();

	virtual void render(const glm::mat4x4 &viewProjectTransform) override;

	void renderedLevel(size_t level);

private:
	std::function<const std::shared_ptr<VortonOctHeap>(void)> m_OctHeapGetter;
	TriangleNetObject m_DrawPrototype;

	size_t m_RenderedLevel;
};

