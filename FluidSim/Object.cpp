#include "Object.h"
#include "glm/gtc/matrix_transform.hpp"

Object::Object(Scene& scene, Material& material, Geometry& geometry, Program& renderProg) : m_Scene{ scene }, m_Material{ material }, m_Geometry{ geometry }, m_RenderProg{renderProg}
{

}


Object::~Object()
{
}

Program& Object::getProgram() const
{
	return m_RenderProg;
}

void Object::translate(const glm::vec3 & delta)
{
	m_ModelTransform = glm::translate(m_ModelTransform, delta);
}

void Object::render(const glm::mat4x4 &viewProjectTransform) const
{
	if (m_RenderProg.getLoadedMaterialId() != m_Material.getId()) {
		m_RenderProg.loadMaterial(m_Material);
	}
	m_RenderProg.loadModelViewProjectTransform(viewProjectTransform*m_ModelTransform);
	m_RenderProg.loadModelTransform(m_ModelTransform);
	m_RenderProg.use();

	m_Geometry.render();
	//m_Geometry.debugRender(viewProjectTransform*m_ModelTransform);
}