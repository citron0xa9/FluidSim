#include "TriangleNetObject.h"

TriangleNetObject::TriangleNetObject(Scene& scene, Material& material, Geometry& geometry, Program& renderProg) : Object{ scene }, m_Material{ material }, m_Geometry{ geometry }, m_RenderProg{renderProg}
{

}


TriangleNetObject::~TriangleNetObject()
{
}

Object * TriangleNetObject::copy() const
{
	return new TriangleNetObject{ *this };
}

Program& TriangleNetObject::getProgram() const
{
	return m_RenderProg;
}

void TriangleNetObject::render(const glm::mat4x4 &viewProjectTransform) const
{
	if (m_RenderProg.getLoadedMaterialId() != m_Material.getId()) {
		m_RenderProg.loadMaterial(m_Material);
	}
	glm::mat4x4 modelTransform = m_TranslationTransform*m_RotationTransform;
	m_RenderProg.loadModelViewProjectTransform(viewProjectTransform*modelTransform);
	m_RenderProg.loadModelTransform(modelTransform);
	m_RenderProg.use();

	m_Geometry.render();
	//m_Geometry.debugRender(viewProjectTransform*m_ModelTransform);
}