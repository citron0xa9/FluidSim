
#include "VortonSim.h"
#include <glm/mat3x3.hpp>
#include "fsmath.h"
#include "Log.h"

const size_t VortonSim::m_VORTONS_PER_DIMENSION = 16;
const size_t VortonSim::m_TRACERS_PER_DIMENSION = m_VORTONS_PER_DIMENSION*2;

VortonSim::VortonSim(double viscosity, double density, const VorticityDistribution &initialVorticity, double vorticityMagnitude, const TriangleNetObject &vortonPrototype)
	: m_Viscosity{ viscosity }, m_Density{ density }, m_VortonHeapPtr{ nullptr }, m_VelocityGridPtr{ nullptr }, m_VortonsRendered{false},
	m_TracersRendered{true}, m_TracerVerticesBuf{false}, m_TracerVao{false}, m_TracerRenderProg{std::vector<ShaderLightSourceVariable>()},
	m_Simulating{true}, m_SimulationTimescale{1.0}
{
	initializeVortons(initialVorticity, vorticityMagnitude, vortonPrototype);
	initializeTracers(initialVorticity);
	setupTracerRenderProgram();
}

VortonSim::VortonSim(const VortonSim & original)
	: m_Viscosity{original.m_Viscosity}, m_Density{original.m_Density}, m_VortonHeapPtr{nullptr}, m_VelocityGridPtr{nullptr},
	m_VortonsRendered{original.m_VortonsRendered}, m_TracersRendered{original.m_TracersRendered}, m_TracerVerticesBuf{false}, m_TracerVao{false},
	m_TracerRenderProg{std::vector<ShaderLightSourceVariable>()}, m_Simulating{true}, m_SimulationTimescale{original.m_SimulationTimescale}
{
	initializeVortons(original);
	initializeTracers(original);
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
	if (m_VelocityGridDrawPtr != nullptr) {
		delete m_VelocityGridDrawPtr;
	}
}

void VortonSim::step(double secondsPassed)
{
	m_InUpdateMutex.lock();
	update(secondsPassed);
	m_InUpdateMutex.unlock();
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
	if ((m_VelocityGridDrawPtr != nullptr) && (m_VelocityGridPtr != nullptr)) {
		m_VelocityGridDrawPtr->updateGeometry(*m_VelocityGridPtr);
		m_VelocityGridDrawPtr->render(viewProjectTransform);
	}
}

void VortonSim::registerSceneHooks(Scene &scene)
{
	ActiveObject::registerSceneHooks(scene);
	DrawableObject::registerSceneHooks(scene);
}

void VortonSim::deregisterSceneHooks(Scene &scene)
{
	ActiveObject::deregisterSceneHooks(scene);
	DrawableObject::deregisterSceneHooks(scene);
}

void VortonSim::update(double seconds)
{
	seconds *= m_SimulationTimescale;
	if (!m_Simulating) {
		return;
	}
	createOctHeap();
	
	calculateVelocityGrid();

	stretchAndTiltVortons(seconds);

	diffuseVorticityPSE(seconds);

	advectVortons(seconds);
	advectTracers(seconds);
}

void VortonSim::vortonsRendered(bool areRendered)
{
	m_VortonsRendered = areRendered;
}

void VortonSim::tracersRendered(bool areRendered)
{
	m_TracersRendered = areRendered;
}

void VortonSim::simulating(bool isSimulating)
{
	m_Simulating = isSimulating;
}

void VortonSim::simulationTimescale(double timescale)
{
	m_SimulationTimescale = timescale;
}

Object * VortonSim::copy() const
{
	WARNING("VortonsSim::copy used");
	return new VortonSim(*this);
}

std::mutex & VortonSim::inUpdateMutex()
{
	return m_InUpdateMutex;
}

void VortonSim::initializeVortons(const VorticityDistribution & initialVorticity, double vorticityMagnitude, const TriangleNetObject &vortonPrototype)
{
	glm::dvec3 minCorner = initialVorticity.minCorner();
	//glm::vec3 maxCorner = minCorner + initialVorticity.getDomainSize();
	glm::dvec3 vortonDistance = initialVorticity.domainSize() / static_cast<double>(m_VORTONS_PER_DIMENSION);
	double vortonRadius = std::cbrt(vortonDistance.x * vortonDistance.y * vortonDistance.z) * 0.5;

	for (int xIndex = 0; xIndex < m_VORTONS_PER_DIMENSION; xIndex++) {
		for (int yIndex = 0; yIndex < m_VORTONS_PER_DIMENSION; yIndex++) {
			for (int zIndex = 0; zIndex < m_VORTONS_PER_DIMENSION; zIndex++) {
				glm::dvec3 position = glm::dvec3(
					vortonDistance.x * xIndex,
					vortonDistance.y * yIndex,
					vortonDistance.z * zIndex);
				position += minCorner;
				Vorton vorton(vortonPrototype, position, initialVorticity.vorticityAtPosition(position) * vorticityMagnitude);
				vorton.radius(vortonRadius);
				if (glm::length(vorton.vorticity()) > SIGNIFICANT_VORTICITY) {
					m_Vortons.push_back(vorton);
				}
			}
		}
	}	
}

void VortonSim::initializeVortons(const VortonSim & original)
{
	for (auto & originalVorton : original.m_Vortons) {
		Vorton vorton(originalVorton);
		m_Vortons.push_back(vorton);
	}
}

void VortonSim::initializeTracers(const VorticityDistribution & initialVorticity)
{
	glm::dvec3 minCorner = initialVorticity.minCorner();
	//glm::vec3 maxCorner = minCorner + initialVorticity.getDomainSize();
	glm::dvec3 tracerDistance = initialVorticity.domainSize() / static_cast<double>(m_TRACERS_PER_DIMENSION);

	for (int xIndex = 0; xIndex < m_TRACERS_PER_DIMENSION; xIndex++) {
		for (int yIndex = 0; yIndex < m_TRACERS_PER_DIMENSION; yIndex++) {
			for (int zIndex = 0; zIndex < m_TRACERS_PER_DIMENSION; zIndex++) {
				glm::dvec3 position = glm::dvec3(
					tracerDistance.x * xIndex,
					tracerDistance.y * yIndex,
					tracerDistance.z * zIndex);
				position += minCorner;
				m_Tracers.emplace_back(*this);
				m_Tracers.back().position(position);
				for (int i = 0; i < 3; i++) {
					m_TracerVerticesRAM.push_back(static_cast<GLfloat>(position[i]));
				}
			}
		}
	}
	m_TracerVerticesBuf.pushData(m_TracerVerticesRAM, GL_DYNAMIC_DRAW, true);
	m_TracerVao.addVertexAttribArray(m_TracerVerticesBuf, true, false, m_TracerRenderProg.vertexPositionIndex(), 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
	m_TracerVao.enableVertexAttribArray(false, m_TracerRenderProg.vertexPositionIndex());
	m_TracerVao.unbind();
}

void VortonSim::initializeTracers(const VortonSim & original)
{
	for (auto & originalTracer : original.m_Tracers) {
		glm::dvec3 position = originalTracer.position();
		m_Tracers.emplace_back(*this);
		m_Tracers.back().position(position);
		for (int i = 0; i < 3; i++) {
			m_TracerVerticesRAM.push_back(static_cast<GLfloat>(position[i]));
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

void VortonSim::createOctHeap()
{
	if (m_VortonHeapPtr != nullptr) {
		delete m_VortonHeapPtr;
	}
	m_VortonHeapPtr = new VortonOctHeap(m_Vortons);
}

void VortonSim::calculateVelocityGrid()
{
	size_t numPoints = m_VORTONS_PER_DIMENSION * m_VORTONS_PER_DIMENSION * m_VORTONS_PER_DIMENSION;
	if (m_VelocityGridPtr != nullptr) {
		delete m_VelocityGridPtr;
	}

	auto gridDimensions = velocityGridDimensions();

	m_VelocityGridPtr = new UniformGrid<glm::dvec3>(numPoints, gridDimensions.first, gridDimensions.second);
	
	//loop over every point in uniform grid
	size_t velocityGridOffset = 0;
	glm::dvec3 currentGridPosition = m_VelocityGridPtr->minCorner();

	for (size_t zIndex = 0; zIndex < m_VelocityGridPtr->pointsAmount().z; zIndex++) {
		for (size_t yIndex = 0; yIndex < m_VelocityGridPtr->pointsAmount().y; yIndex++) {
			for (size_t xIndex = 0; xIndex < m_VelocityGridPtr->pointsAmount().x; xIndex++) {
				m_VelocityGridPtr->atOffset(velocityGridOffset) = calculateVelocity(currentGridPosition);
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

glm::dvec3 VortonSim::calculateVelocity(const glm::dvec3 & position)
{
	assert(m_VortonHeapPtr != nullptr);
	return (m_VortonHeapPtr->root().calculateVelocity(position));
}

void VortonSim::stretchAndTiltVortons(double seconds)
{
	UniformGrid<glm::dmat3x3> velocityJacobianGrid(*m_VelocityGridPtr);
	fsmath::computeJacobian(velocityJacobianGrid, *m_VelocityGridPtr);

	for (auto &vorton : m_Vortons) {
		glm::dmat3x3 velocityJacobian = velocityJacobianGrid.interpolate(vorton.position());

		glm::dvec3 stretchTilt = vorton.vorticity() * velocityJacobian;
		glm::dvec3 newVorticity = vorton.vorticity() + 0.5 * stretchTilt * seconds;	//0.5 is fudge factor for stability

		vorton.vorticity(newVorticity);
	}
}

void VortonSim::diffuseVorticityPSE(double seconds)
{
	for (auto leafs = m_VortonHeapPtr->leafs(); leafs.first != leafs.second; leafs.first++) {
		Supervorton &currentSupervorton = leafs.first->supervorton();
		diffuseVorticityInside(seconds, currentSupervorton.containedVortonPtrs());

		for (auto neighborPtr : leafs.first->forwardNeighbors()) {
			Supervorton &neighborSupervorton = neighborPtr->supervorton();
			diffuseVorticityBetween(seconds, currentSupervorton.containedVortonPtrs(), neighborSupervorton.containedVortonPtrs());
		}

		//dissipate vorticity
		for (auto vortonPtr : currentSupervorton.containedVortonPtrs()) {
			vortonPtr->vorticity(vortonPtr->vorticity() - (vortonPtr->vorticity() * seconds * m_Viscosity));
		}
	}
}

void VortonSim::diffuseVorticityInside(double seconds, std::vector<Vorton*>& vortonPtrs)
{
	for (auto outerIterator = vortonPtrs.begin(); outerIterator != vortonPtrs.end(); outerIterator++) {
		for (auto innerIterator = outerIterator + 1; innerIterator < vortonPtrs.end(); innerIterator++) {
			diffuseVorticityBetween(seconds, **outerIterator, **innerIterator);
		}
	}
}

void VortonSim::diffuseVorticityBetween(double seconds, std::vector<Vorton*>& firstVortonPtrs, std::vector<Vorton*>& secondVortonPtrs)
{
	for (auto outerIterator = firstVortonPtrs.begin(); outerIterator != firstVortonPtrs.end(); outerIterator++) {
		for (auto innerIterator = secondVortonPtrs.begin(); innerIterator != secondVortonPtrs.end(); innerIterator++) {
			diffuseVorticityBetween(seconds, **outerIterator, **innerIterator);
		}
	}
}

void VortonSim::diffuseVorticityBetween(double seconds, Vorton & first, Vorton & second)
{
	glm::dvec3 vorticityDifference = first.vorticity() - second.vorticity();
	glm::dvec3 exchange = 2 * m_Viscosity * seconds * vorticityDifference;
	first.vorticity(first.vorticity() - exchange);
	second.vorticity(second.vorticity() + exchange);
}

void VortonSim::advectVortons(double secondsPassed)
{
	for (auto &vorton : m_Vortons) {
		glm::dvec3 positionChange = m_VelocityGridPtr->interpolate(vorton.position()) * secondsPassed;
		vorton.translate(positionChange);
	}
}

void VortonSim::advectTracers(double secondsPassed)
{

	for (int i = 0; i < m_Tracers.size(); i++) {
		glm::vec3 positionChange = m_VelocityGridPtr->interpolate(m_Tracers[i].position()) * secondsPassed;
		m_Tracers[i].translate(positionChange);

		//update m_TracerVericesRAM
		glm::dvec3 tracerPosition = m_Tracers[i].position();
		size_t iterationCounter = 0;
		for (int j = i*3; j < (i*3+3); j++) {
			m_TracerVerticesRAM[j] = static_cast<GLfloat>(tracerPosition[iterationCounter++]);
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
	glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_Tracers.size()));
	m_TracerVao.unbind();
	
}

std::pair<glm::dvec3, glm::dvec3> VortonSim::velocityGridDimensions()
{
	//find min corner and max corner
	glm::dvec3 minCorner = glm::dvec3(FLT_MAX);
	glm::dvec3 maxCorner = glm::dvec3(FLT_MIN);

	for (auto & tracer : m_Tracers) {
		minCorner = fsmath::allMin(tracer.position(), minCorner);
		maxCorner = fsmath::allMax(tracer.position(), maxCorner);
	}

	for (auto & vorton : m_Vortons) {
		minCorner = fsmath::allMin(vorton.position(), minCorner);
		maxCorner = fsmath::allMax(vorton.position(), maxCorner);
	}
	minCorner -= glm::dvec3(FLT_EPSILON);
	maxCorner += glm::dvec3(FLT_EPSILON);

	//return dimensions as minCorner, maxCorner
	return std::make_pair(minCorner, maxCorner);
}

void VortonSim::showVelocityGrid()
{
	m_VelocityGridDrawPtr = new DrawableGridGeometry();
}

void VortonSim::hideVelocityGrid()
{
	if (m_VelocityGridDrawPtr != nullptr) {
		delete m_VelocityGridDrawPtr;
		m_VelocityGridDrawPtr = nullptr;
	}
}
