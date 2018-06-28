
#include "VortonRendererOctHeap.h"
#include "VortonOctHeapElement.h"
#include "Scene.h"
#include "Supervorton.h"

VortonRendererOctHeap::VortonRendererOctHeap(Scene & scene, Program & phongProgram, const oct_heap_getter_t& octHeapGetter)
	: m_OctHeapGetter{ octHeapGetter }, m_DrawPrototype{}, m_RenderedLevel{0}
{
	/*
	* Setup m_DrawPrototype
	*/
	Material sphereMaterial{ glm::vec3(0.0f, 0.8f, 0.8f), glm::vec3(0.0f, 0.8f, 0.8f), glm::vec3(0.2f), 10 };
	Material &sphereMatRef = scene.addMaterial(sphereMaterial);

	//setup geometry
	Geometry &geomSphere = scene.addGeometryFromFile("objects\\sphere.obj");
	geomSphere.setupAttribArrays(phongProgram);

	//create vorton prototype
	m_DrawPrototype.material(&sphereMatRef);
	m_DrawPrototype.geometry(&geomSphere);
	m_DrawPrototype.program(&phongProgram);
	m_DrawPrototype.scale(glm::dvec3(0.08));
}

VortonRendererOctHeap::~VortonRendererOctHeap()
{
}

void VortonRendererOctHeap::render(const glm::mat4x4 & viewProjectTransform)
{
	const auto& octHeap = m_OctHeapGetter();
	if (octHeap == nullptr) {
		return;
	}

	if (octHeap->divisions() < m_RenderedLevel) {
		std::string msg = "OctHeapRenderer: Divisions of oct heap is: " + std::to_string(octHeap->divisions()) + ".\n";
		msg += "Won't display level: " + std::to_string(m_RenderedLevel) + ".\n";
		msg += "Setting renderedLevel back to: " + std::to_string(octHeap->divisions()) + " and displaying this level.";
		WARNING(msg);
		m_RenderedLevel = octHeap->divisions();
	}
	

	for (auto vortonsToRender = octHeap->allAtLevel(m_RenderedLevel); vortonsToRender.first != vortonsToRender.second; vortonsToRender.first++) {
		m_DrawPrototype.position(vortonsToRender.first->supervorton().position());
		m_DrawPrototype.renderWithId(viewProjectTransform, vortonsToRender.first->supervorton().id());
	}

}

void VortonRendererOctHeap::renderedLevel(size_t level)
{
	m_RenderedLevel = level;
}
