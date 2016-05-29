
#include "VortonRenderer.h"
#include "Scene.h"


VortonRenderer::VortonRenderer(Scene &scene, const std::vector<Vorton>& baseVortons)
	: m_BaseVortons{ baseVortons }, m_DrawPrototype{}
{
	/*
	 * Setup m_DrawPrototype
	*/
	Material sphereMaterial{ glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0.2f), 10 };
	Material &sphereMatRef = scene.addMaterial(sphereMaterial);

	ShaderLightSourceVariable lightSrcVar{ "sunLight", LightSourceType::SUNLIGHT };
	std::vector<ShaderLightSourceVariable> lightSrcVars;
	lightSrcVars.push_back(lightSrcVar);

	Program &prog = scene.addProgram(lightSrcVars);

	Shader vertexShader{ GL_VERTEX_SHADER };
	vertexShader.loadSourceFromFile("shaders\\basic3D.vert");

	Shader fragmentShader{ GL_FRAGMENT_SHADER };
	fragmentShader.loadSourceFromFile("shaders\\basic3D_phong.frag");

	prog.attachShader(&vertexShader);
	prog.attachShader(&fragmentShader);

	vertexShader.compile();
	fragmentShader.compile();
	prog.link();
	prog.detachAllShaders();

	//setup geometry
	Geometry &geomSphere = scene.addGeometryFromFile("objects\\sphere.obj");
	geomSphere.setupAttribArrays(prog);

	//create vorton prototype
	m_DrawPrototype.material(&sphereMatRef);
	m_DrawPrototype.geometry(&geomSphere);
	m_DrawPrototype.program(&prog);
	m_DrawPrototype.scale(glm::dvec3(0.03));

	scene.loadLightsIntoPrograms();
}

VortonRenderer::~VortonRenderer()
{
}

void VortonRenderer::render(const glm::mat4x4 & viewProjectTransform)
{
	for (auto &vorton : m_BaseVortons) {
		m_DrawPrototype.position(vorton.position());
		m_DrawPrototype.render(viewProjectTransform);
	}
}
