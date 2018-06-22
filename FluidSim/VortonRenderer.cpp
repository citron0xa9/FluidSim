
#include "VortonRenderer.h"
#include "Scene.h"


VortonRenderer::VortonRenderer(Scene &scene, const std::vector<std::unique_ptr<Particle>>& baseVortonPtrs, Program &phongProgram)
	: m_BaseVortonPtrs{ baseVortonPtrs }, m_DrawPrototype{}
{
	/*
	 * Setup m_DrawPrototype
	*/
	Material sphereMaterial{ glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0.2f), 10 };
	Material &sphereMatRef = scene.addMaterial(sphereMaterial);

	//setup geometry
	Geometry &geomSphere = scene.addGeometryFromFile("objects\\sphere.obj");
	geomSphere.setupAttribArrays(phongProgram);

	//create vorton prototype
	m_DrawPrototype.material(&sphereMatRef);
	m_DrawPrototype.geometry(&geomSphere);
	m_DrawPrototype.program(&phongProgram);
	m_DrawPrototype.scale(glm::dvec3(0.03));
}

VortonRenderer::~VortonRenderer()
{
}

void VortonRenderer::render(const glm::mat4x4 & viewProjectTransform)
{
	for (auto &vortonPtr : m_BaseVortonPtrs) {
		m_DrawPrototype.position(vortonPtr->position());
		m_DrawPrototype.renderWithId(viewProjectTransform, vortonPtr->id());
	}
}
