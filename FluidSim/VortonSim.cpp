
#include "VortonSim.h"
#include <glm/mat3x3.hpp>
#include "fsmath.h"

const size_t VortonSim::m_VORTONS_PER_DIMENSION = 16;
const size_t VortonSim::m_TRACERS_PER_DIMENSION = m_VORTONS_PER_DIMENSION*2;

#if 0
VortonSim::VortonSim(const VortonSim & sim)
	: ActiveObject( sim ), DrawableObject( sim ), Object( sim ), ContainerObject(),	//dont copy container data
	//copy first part of sim data
	m_Viscosity{ sim.m_Viscosity }, m_Density{ sim.m_Density }, m_Vortons{ sim.m_Vortons }, m_Tracers{ sim.m_Tracers }, m_VortonsRendered {sim.m_VortonsRendered},
	m_TracersRendered{sim.m_TracersRendered}, m_VortonHeapPtr{nullptr}, m_VelocityGridPtr{nullptr}, m_TracerRenderProgPtr{ sim.m_TracerRenderProgPtr }
{
	//copy container data
	//copy objects
	for (auto objectPtr : sim.m_ObjectPtrs) {
		Object *copiedObject = objectPtr->copy();
		m_ObjectPtrs.push_back(copiedObject);
		copiedObject->setContainerObject(*this);
		copiedObject->registerContainerObjectHooks();
	}
}
#endif

VortonSim::VortonSim(ContainerObject &container, float viscosity, float density, const VorticityDistribution &initialVorticity, float vorticityMagnitude, const TriangleNetObject &vortonPrototype)
	: ActiveObject{ container }, DrawableObject{ container }, Object{ container }, m_Viscosity{ viscosity }, m_Density{ density }, m_VortonHeapPtr{ nullptr }, m_VelocityGridPtr{ nullptr },
	m_VortonsRendered{false}, m_TracersRendered{true}, m_TracerVerticesBuf{false}, m_TracerVao{false}, m_TracerRenderProg{std::vector<ShaderLightSourceVariable>()}
{
	initializeVortons(initialVorticity, vorticityMagnitude, vortonPrototype);
	initializeTracers(initialVorticity);
	setupTracerRenderProgram();
}

VortonSim::~VortonSim()
{
	if (m_VortonHeapPtr != nullptr) {
		delete m_VortonHeapPtr;
	}
	if (m_VelocityGridPtr != nullptr) {
		delete m_VelocityGridPtr;
	}
}

void VortonSim::step(float secondsPassed)
{
	ContainerObject::step(secondsPassed);
	Update(secondsPassed);
}

void VortonSim::render(const glm::mat4x4 & viewProjectTransform)
{
	if (m_VortonsRendered) {
		for (auto & vorton : m_Vortons) {
			vorton.render(viewProjectTransform);
		}
	}

	if (m_TracersRendered) {
		renderTracers(viewProjectTransform);
	}
	ContainerObject::render(viewProjectTransform);
}

void VortonSim::registerContainerObjectHooks()
{
	ActiveObject::registerContainerObjectHooks();
	DrawableObject::registerContainerObjectHooks();
}

void VortonSim::Update(float seconds)
{
	CreateOctHeap();
	
	CalculateVelocityGrid();

	StretchAndTiltVortons(seconds);

	DiffuseVorticityPSE(seconds);

	advectVortons(seconds);
	advectTracers(seconds);
}

void VortonSim::setVortonsRendered(bool areRendered)
{
	m_VortonsRendered = areRendered;
}

void VortonSim::setTracersRendered(bool areRendered)
{
	m_TracersRendered = areRendered;
}

Object * VortonSim::copy() const
{
	return nullptr;
}

void VortonSim::initializeVortons(const VorticityDistribution & initialVorticity, float vorticityMagnitude, const TriangleNetObject &vortonPrototype)
{
	glm::vec3 minCorner = initialVorticity.minCorner();
	//glm::vec3 maxCorner = minCorner + initialVorticity.getDomainSize();
	glm::vec3 vortonDistance = initialVorticity.getDomainSize() / static_cast<float>(m_VORTONS_PER_DIMENSION);
	float vortonRadius = std::cbrt(vortonDistance.x * vortonDistance.y * vortonDistance.z) * 0.5f;

	for (int xIndex = 0; xIndex < m_VORTONS_PER_DIMENSION; xIndex++) {
		for (int yIndex = 0; yIndex < m_VORTONS_PER_DIMENSION; yIndex++) {
			for (int zIndex = 0; zIndex < m_VORTONS_PER_DIMENSION; zIndex++) {
				glm::vec3 position = glm::vec3(
					vortonDistance.x * xIndex,
					vortonDistance.y * yIndex,
					vortonDistance.z * zIndex);
				position += minCorner;
				Vorton vorton(vortonPrototype, position, initialVorticity.getVorticityAtPosition(position) * vorticityMagnitude);
				vorton.setContainerObject(*this);
				vorton.radius(vortonRadius);
				if (glm::length(vorton.vorticity()) > SIGNIFICANT_VORTICITY) {
					m_Vortons.push_back(vorton);
				}
			}
		}
	}	
}

void VortonSim::initializeTracers(const VorticityDistribution & initialVorticity)
{
	glm::vec3 minCorner = initialVorticity.minCorner();
	//glm::vec3 maxCorner = minCorner + initialVorticity.getDomainSize();
	glm::vec3 tracerDistance = initialVorticity.getDomainSize() / static_cast<float>(m_TRACERS_PER_DIMENSION);

	for (int xIndex = 0; xIndex < m_TRACERS_PER_DIMENSION; xIndex++) {
		for (int yIndex = 0; yIndex < m_TRACERS_PER_DIMENSION; yIndex++) {
			for (int zIndex = 0; zIndex < m_TRACERS_PER_DIMENSION; zIndex++) {
				glm::vec3 position = glm::vec3(
					tracerDistance.x * xIndex,
					tracerDistance.y * yIndex,
					tracerDistance.z * zIndex);
				position += minCorner;
				m_Tracers.emplace_back(*this);
				m_Tracers.back().setPosition(position);
				for (int i = 0; i < 3; i++) {
					m_TracerVerticesRAM.push_back(position[i]);
				}
			}
		}
	}
	m_TracerVerticesBuf.pushData(m_TracerVerticesRAM, GL_DYNAMIC_DRAW, true);
	m_TracerVao.addVertexAttribArray(m_TracerVerticesBuf, true, false, m_TracerRenderProg.vertexPositionIndex(), 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
	m_TracerVao.enableVertexAttribArray(false, m_TracerRenderProg.vertexPositionIndex());
	m_TracerVao.unbind();
}

void VortonSim::setupTracerRenderProgram()
{
	Shader vertexShader{ GL_VERTEX_SHADER };
	vertexShader.loadSourceFromFile("shaders\\tracer.vert");

	Shader fragmentShader{ GL_FRAGMENT_SHADER };
	fragmentShader.loadSourceFromFile("shaders\\tracer.frag");

	m_TracerRenderProg.attachShader(&vertexShader);
	m_TracerRenderProg.attachShader(&fragmentShader);

	vertexShader.compile();
	fragmentShader.compile();
	m_TracerRenderProg.link();
	m_TracerRenderProg.detachAllShaders();
}

void VortonSim::CreateOctHeap()
{
	if (m_VortonHeapPtr != nullptr) {
		delete m_VortonHeapPtr;
	}
	m_VortonHeapPtr = new VortonOctHeap(m_Vortons);
}

void VortonSim::CalculateVelocityGrid()
{
	size_t numPoints = m_VORTONS_PER_DIMENSION * m_VORTONS_PER_DIMENSION * m_VORTONS_PER_DIMENSION;
	if (m_VelocityGridPtr != nullptr) {
		delete m_VelocityGridPtr;
	}

	auto gridDimensions = getVelocityGridDimensions();

	m_VelocityGridPtr = new UniformGrid<glm::vec3>(numPoints, gridDimensions.first, gridDimensions.second, false);
	
	//loop over every point in uniform grid
	size_t velocityGridOffset = 0;
	glm::vec3 currentGridPosition = m_VelocityGridPtr->minCorner();

	for (size_t zIndex = 0; zIndex < m_VelocityGridPtr->pointsAmount().z; zIndex++) {
		for (size_t yIndex = 0; yIndex < m_VelocityGridPtr->pointsAmount().y; yIndex++) {
			for (size_t xIndex = 0; xIndex < m_VelocityGridPtr->pointsAmount().x; xIndex++) {
				m_VelocityGridPtr->atOffset(velocityGridOffset) = CalculateVelocity(currentGridPosition);
				velocityGridOffset++;
				currentGridPosition.x += m_VelocityGridPtr->cellExtent().x;
			}
			currentGridPosition.x = m_VelocityGridPtr->minCorner().x;
			currentGridPosition.y += m_VelocityGridPtr->cellExtent().y;
		}
		currentGridPosition.y = m_VelocityGridPtr->minCorner().y;
		currentGridPosition.z += m_VelocityGridPtr->cellExtent().z;
	}
}

glm::vec3 VortonSim::CalculateVelocity(const glm::vec3 & position)
{
	assert(m_VortonHeapPtr != nullptr);
	return (m_VortonHeapPtr->root().calculateVelocity(position));
}

void VortonSim::StretchAndTiltVortons(float seconds)
{
	UniformGrid<glm::mat3x3> velocityJacobianGrid(*m_VelocityGridPtr);
	fsmath::computeJacobian(velocityJacobianGrid, *m_VelocityGridPtr);

	for (auto &vorton : m_Vortons) {
		glm::mat3x3 velocityJacobian = velocityJacobianGrid.interpolate(vorton.getPosition());

		glm::vec3 stretchTilt = vorton.vorticity() * velocityJacobian;
		glm::vec3 newVorticity = vorton.vorticity() + 0.5f * stretchTilt * seconds;	//0.5 is fudge factor for stability

		vorton.vorticity(newVorticity);
	}
}

void VortonSim::DiffuseVorticityPSE(float seconds)
{
	for (auto leafs = m_VortonHeapPtr->leafs(); leafs.first != leafs.second; leafs.first++) {
		Supervorton &currentSupervorton = leafs.first->supervorton();
		DiffuseVorticityInside(seconds, currentSupervorton.containedVortonPtrs());

		for (auto neighborPtr : leafs.first->forwardNeighbors()) {
			Supervorton &neighborSupervorton = neighborPtr->supervorton();
			DiffuseVorticityBetween(seconds, currentSupervorton.containedVortonPtrs(), neighborSupervorton.containedVortonPtrs());
		}

		//dissipate vorticity
		for (auto vortonPtr : currentSupervorton.containedVortonPtrs()) {
			vortonPtr->vorticity(vortonPtr->vorticity() - (vortonPtr->vorticity() * seconds * m_Viscosity));
		}
	}
}

void VortonSim::DiffuseVorticityInside(float seconds, std::vector<Vorton*>& vortonPtrs)
{
	for (auto outerIterator = vortonPtrs.begin(); outerIterator != vortonPtrs.end(); outerIterator++) {
		for (auto innerIterator = outerIterator + 1; innerIterator < vortonPtrs.end(); innerIterator++) {
			DiffuseVorticityBetween(seconds, **outerIterator, **innerIterator);
		}
	}
}

void VortonSim::DiffuseVorticityBetween(float seconds, std::vector<Vorton*>& firstVortonPtrs, std::vector<Vorton*>& secondVortonPtrs)
{
	for (auto outerIterator = firstVortonPtrs.begin(); outerIterator != firstVortonPtrs.end(); outerIterator++) {
		for (auto innerIterator = secondVortonPtrs.begin(); innerIterator != secondVortonPtrs.end(); innerIterator++) {
			DiffuseVorticityBetween(seconds, **outerIterator, **innerIterator);
		}
	}
}

void VortonSim::DiffuseVorticityBetween(float seconds, Vorton & first, Vorton & second)
{
	glm::vec3 vorticityDifference = first.vorticity() - second.vorticity();
	glm::vec3 exchange = 2 * m_Viscosity * seconds * vorticityDifference;
	first.vorticity(first.vorticity() - exchange);
	second.vorticity(second.vorticity() + exchange);
}

void VortonSim::advectVortons(float secondsPassed)
{
	for (auto &vorton : m_Vortons) {
		glm::vec3 positionChange = m_VelocityGridPtr->interpolate(vorton.getPosition()) * secondsPassed;
		vorton.translate(positionChange);
	}
}

void VortonSim::advectTracers(float secondsPassed)
{

	for (int i = 0; i < m_Tracers.size(); i++) {
		glm::vec3 positionChange = m_VelocityGridPtr->interpolate(m_Tracers[i].getPosition()) * secondsPassed;
		m_Tracers[i].translate(positionChange);

		//update m_TracerVericesRAM
		glm::vec3 tracerPosition = m_Tracers[i].getPosition();
		size_t iterationCounter = 0;
		for (int j = i*3; j < (i*3+3); j++) {
			m_TracerVerticesRAM[j] = tracerPosition[iterationCounter++];
		}
	}
	/*m_TracerVao.bind();
	m_TracerVerticesBuf.pushDataSubset(m_TracerVerticesRAM, 0, m_TracerVerticesRAM.size(), true);*/
	//m_TracerVerticesBuf.pushData(m_TracerVerticesRAM, GL_DYNAMIC_DRAW, true);
}

void VortonSim::renderTracers(const glm::mat4x4 & viewProjectTransform)
{
	m_TracerVao.bind();

	m_TracerVerticesBuf.pushDataSubset(m_TracerVerticesRAM, 0, m_TracerVerticesRAM.size(), true);

	m_TracerRenderProg.loadModelViewProjectTransform(viewProjectTransform);
	m_TracerRenderProg.use();
	glDrawArrays(GL_POINTS, 0, m_Tracers.size());
	m_TracerVao.unbind();
	
}

std::pair<glm::vec3, glm::vec3> VortonSim::getVelocityGridDimensions()
{
	//find min corner and max corner
	glm::vec3 minCorner = m_VortonHeapPtr->minCorner();
	glm::vec3 maxCorner = minCorner + m_VortonHeapPtr->extent();

	for (auto & tracer : m_Tracers) {
		minCorner = fsmath::allMin(tracer.getPosition(), minCorner);
		maxCorner = fsmath::allMax(tracer.getPosition(), maxCorner);
	}

	for (auto & vorton : m_Vortons) {
		minCorner = fsmath::allMin(vorton.getPosition(), minCorner);
		maxCorner = fsmath::allMax(vorton.getPosition(), maxCorner);
	}

	//return dimensions as minCorner, maxCorner
	return std::make_pair(minCorner, maxCorner);
}
