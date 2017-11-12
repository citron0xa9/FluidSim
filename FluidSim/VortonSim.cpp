
#include "VortonSim.h"
#include <glm/mat3x3.hpp>
#include <glm/gtx/string_cast.hpp>
#include "fsmath.h"
#include "Log.h"

VortonSim::VortonSim(RigidBodySim& rbSim, double viscosity, double density, const VorticityDistribution &initialVorticity, double vorticityMagnitude)
	: VortonSim{rbSim, viscosity, density, std::vector<const VorticityDistribution*>{&initialVorticity}, vorticityMagnitude}
{
}


VortonSim::VortonSim(RigidBodySim& rbSim, double viscosity, double density, const std::vector<const VorticityDistribution*> &initialVorticityPtrs, double vorticityMagnitude)
	: m_Viscosity{ viscosity }, m_Density{ density }, m_VortonHeapPtr{ nullptr }, m_VelocityGridPtr{ nullptr },
	m_Simulating{true}, m_SimulationTimescale{1.0}, m_RigidBodySim{rbSim}
{

	for (auto initialVorticityPtr : initialVorticityPtrs) {
        const auto tracersSizeBefore = m_Tracers.size();
        const auto vortonsSizeBefore = m_Vortons.size();

		initializeVortons(*initialVorticityPtr, vorticityMagnitude);
		initializeTracers(*initialVorticityPtr);

        const auto addedVortons = m_Vortons.size() - vortonsSizeBefore;
        const auto addedObjectsAmount = m_Tracers.size() - tracersSizeBefore + addedVortons;

        const auto domainExtent = initialVorticityPtr->domainSize();
        const double volume = domainExtent.x * domainExtent.y * domainExtent.z;
        const auto massPerObject = volume * density / addedObjectsAmount;

        for (auto it = m_Tracers.begin()+tracersSizeBefore; it != m_Tracers.end(); it++) {
            it->mass(massPerObject);
        }

        for (auto it = m_Vortons.begin()+vortonsSizeBefore; it != m_Vortons.end(); it++) {
            it->mass(massPerObject);
        }
	}

    //initializeRigidBodies();
    removeRigidBodyEmbeddedObjects();
}

VortonSim::VortonSim(const VortonSim & original)
	: m_Viscosity{original.m_Viscosity}, m_Density{original.m_Density}, m_VortonHeapPtr{nullptr}, m_VelocityGridPtr{nullptr},
	m_Simulating{true}, m_SimulationTimescale{original.m_SimulationTimescale}, m_Vortons{original.m_Vortons}, m_Tracers{original.m_Tracers}, m_RigidBodySim{original.m_RigidBodySim}
{
}

VortonSim::~VortonSim()
{
	
}

void VortonSim::step(double secondsPassed)
{
	m_InUpdateMutex.lock();
	update(secondsPassed);
	m_InUpdateMutex.unlock();
}

bool lessThan(const glm::dvec3& first, const glm::dvec3& second) {
    if (first.x < second.x) {
        return true;
    }
    if (first.x == second.x && first.y < second.y) {
        return true;
    }
    if (first.x == second.x && first.y == second.y && first.z < second.z) {
        return true;
    }
    return false;
}

void VortonSim::update(double seconds)
{
	seconds *= m_SimulationTimescale;
	if (!m_Simulating) {
		return;
	}

    static bool firstUpdate = false;

	createOctHeap();
	
	calculateVelocityGrid();
#if 0
    if (firstUpdate) {
        firstUpdate = false;
        std::cout << "First update, dumping data..." << std::endl;

        std::cout << "Vortons:" << std::endl;
        std::vector<std::pair<glm::dvec3, glm::dvec3>> dataToOutput;
        for (auto& vorton : m_Vortons) {
            dataToOutput.emplace_back(vorton.position(), vorton.vorticity());
        }
        std::sort(dataToOutput.begin(), dataToOutput.end(), [](const std::pair<glm::dvec3, glm::dvec3>& first, const std::pair<glm::dvec3, glm::dvec3>& second) {
            return lessThan(first.first, second.first);
        });

        for (const auto& element : dataToOutput) {
            std::cout << glm::to_string(element.first) << ";" << glm::to_string(element.second) << std::endl;
        }
        dataToOutput.clear();

        std::cout << "\n\n\nVelocity grid:" << std::endl;
        size_t offset = 0;
        glm::dvec3 position = m_VelocityGridPtr->minCorner();
        for (size_t z = 0; z < m_VelocityGridPtr->pointsAmount().z; z++) {
            for (size_t y = 0; y < m_VelocityGridPtr->pointsAmount().y; y++) {
                for (size_t x = 0; x < m_VelocityGridPtr->pointsAmount().x; x++) {
                    dataToOutput.emplace_back(position, m_VelocityGridPtr->atOffset(offset));
                    offset++;
                    position.x += m_VelocityGridPtr->cellExtent().x;
                }
                position.x = m_VelocityGridPtr->minCorner().x;
                position.y += m_VelocityGridPtr->cellExtent().y;
            }
            position.x = m_VelocityGridPtr->minCorner().x;
            position.y = m_VelocityGridPtr->minCorner().y;
            position.z += m_VelocityGridPtr->cellExtent().z;
        }

        std::sort(dataToOutput.begin(), dataToOutput.end(), [](const std::pair<glm::dvec3, glm::dvec3>& first, const std::pair<glm::dvec3, glm::dvec3>& second) {
            return lessThan(first.first, second.first);
        });

        for (const auto& element : dataToOutput) {
            std::cout << glm::to_string(element.first) << ";" << glm::to_string(element.second) << std::endl;
        }
    }
#endif

	stretchAndTiltVortons(seconds);

	diffuseVorticityPSE(seconds);

	advectVortons(seconds);
	advectTracers(seconds);

    solveBoundaryConditions();
}

const std::vector<Tracer>& VortonSim::tracers() const
{
	return m_Tracers;
}

const std::vector<Vorton>& VortonSim::vortons() const
{
	return m_Vortons;
}

const std::shared_ptr<UniformGrid<glm::dvec3>> VortonSim::velocityGridPtr() const
{
	return m_VelocityGridPtr;
}

const std::shared_ptr<VortonOctHeap> VortonSim::vortonOctHeapPtr() const
{
	return m_VortonHeapPtr;
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

void VortonSim::initializeVortons(const VorticityDistribution & initialVorticity, double vorticityMagnitude)
{
	const glm::dvec3 minCorner = initialVorticity.minCorner();
	const glm::dvec3 maxCorner = minCorner + initialVorticity.domainSize();
    const auto extent = initialVorticity.domainSize();
    const auto volumePerVorton = extent.x * extent.y * extent.z / static_cast<double>(m_VORTONS_TOTAL);
    const double vortonDistance = std::cbrt(volumePerVorton);
	double vortonRadius = vortonDistance * 0.5;

	for (double xPosition = minCorner.x; xPosition < maxCorner.x; xPosition+=vortonDistance) {
		for (double yPosition = minCorner.y; yPosition < maxCorner.y; yPosition+=vortonDistance) {
			for (double zPosition = minCorner.z; zPosition < maxCorner.z; zPosition+=vortonDistance) {
				//glm::dvec3 position = glm::dvec3(
				//	vortonDistance.x * xIndex,
				//	vortonDistance.y * yIndex,
				//	vortonDistance.z * zIndex);
				//position += minCorner;
                glm::dvec3 position{ xPosition, yPosition, zPosition };
				Vorton vorton(position, initialVorticity.vorticityAtPosition(position) * vorticityMagnitude, vortonRadius, 0.0);
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
    const auto extent = initialVorticity.domainSize();
	const glm::dvec3 minCorner = initialVorticity.minCorner();
	const glm::dvec3 maxCorner = minCorner + initialVorticity.domainSize();

    const auto volumePerTracer = extent.x * extent.y * extent.z / static_cast<double>(m_TRACERS_TOTAL);

    const double tracerDistance = std::cbrt(volumePerTracer);
    const auto tracerRadius = tracerDistance * 0.5;

    for (double xPosition = minCorner.x; xPosition < maxCorner.x; xPosition += tracerDistance) {
        for (double yPosition = minCorner.y; yPosition < maxCorner.y; yPosition += tracerDistance) {
            for (double zPosition = minCorner.z; zPosition < maxCorner.z; zPosition += tracerDistance) {
				//glm::dvec3 position = glm::dvec3(
				//	tracerDistance.x * xIndex,
				//	tracerDistance.y * yIndex,
				//	tracerDistance.z * zIndex);
				//position += minCorner;
                glm::dvec3 position{ xPosition, yPosition, zPosition };
				m_Tracers.emplace_back(tracerRadius);
				m_Tracers.back().position(position);
			}
		}
	}
}

/*void VortonSim::initializeTracers(const VortonSim & original)
{
	for (auto & originalTracer : original.m_Tracers) {
		glm::dvec3 position = originalTracer.position();
		m_Tracers.emplace_back(*this);
		m_Tracers.back().position(position);
	}
}*/

void VortonSim::intializeRigidBodies(const VortonSim & original)
{
}

void VortonSim::removeRigidBodyEmbeddedObjects()
{
    for (const auto& sphere : m_RigidBodySim.spheres()) {
        auto newEndVortons = std::remove_if(m_Vortons.begin(), m_Vortons.end(), [sphere](const Vorton& vorton){
            return glm::length(vorton.position() - sphere.position()) < (vorton.radius() + sphere.radius());
        });
        m_Vortons.erase(newEndVortons, m_Vortons.end());

        auto newEndTracers = std::remove_if(m_Tracers.begin(), m_Tracers.end(), [sphere](const Object& tracer) {
            return glm::length(tracer.position() - sphere.position()) < sphere.radius();
        });
        m_Tracers.erase(newEndTracers, m_Tracers.end());
    }
}

void VortonSim::createOctHeap()
{
	m_VortonHeapPtr = std::make_shared<VortonOctHeap>(m_Vortons);
}

void VortonSim::calculateVelocityGrid()
{
	size_t numPoints = m_VORTONS_PER_DIMENSION * m_VORTONS_PER_DIMENSION * m_VORTONS_PER_DIMENSION;

	auto gridDimensions = velocityGridDimensions();

	auto velocityGridPtr = std::make_shared<UniformGrid<glm::dvec3>>(numPoints, gridDimensions.first, gridDimensions.second);
	
	//loop over every point in uniform grid
	size_t velocityGridOffset = 0;
	glm::dvec3 currentGridPosition = velocityGridPtr->minCorner();

	for (size_t zIndex = 0; zIndex < velocityGridPtr->pointsAmount().z; zIndex++) {
		for (size_t yIndex = 0; yIndex < velocityGridPtr->pointsAmount().y; yIndex++) {
			for (size_t xIndex = 0; xIndex < velocityGridPtr->pointsAmount().x; xIndex++) {
				velocityGridPtr->atOffset(velocityGridOffset) = calculateVelocity(currentGridPosition);
				velocityGridOffset++;
				currentGridPosition.x += velocityGridPtr->cellExtent().x;
			}
			currentGridPosition.x = velocityGridPtr->minCorner().x;
			currentGridPosition.y += velocityGridPtr->cellExtent().y;
		}
		currentGridPosition.y = velocityGridPtr->minCorner().y;
		currentGridPosition.z += velocityGridPtr->cellExtent().z;
	}
	m_VelocityGridPtr = velocityGridPtr;
}

glm::dvec3 VortonSim::calculateVelocity(const glm::dvec3 & position)
{
	if (m_Vortons.empty()) {
		return glm::dvec3(0);
	}
	assert(m_VortonHeapPtr != nullptr);
	static const double ONE_OVER_FOUR_PI = glm::one_over_pi<double>() * 0.25;
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
        const auto vortonVelocity = m_VelocityGridPtr->interpolate(vorton.position());
		glm::dvec3 positionChange = vortonVelocity * secondsPassed;
		vorton.translate(positionChange);

        vorton.velocity(vortonVelocity);
	}
}

void VortonSim::advectTracers(double secondsPassed)
{

	for (int i = 0; i < m_Tracers.size(); i++) {
        const auto tracerVelocity = m_VelocityGridPtr->interpolate(m_Tracers[i].position());
		glm::vec3 positionChange = tracerVelocity * secondsPassed;
		m_Tracers[i].translate(positionChange);

        m_Tracers[i].velocity(tracerVelocity);

		//update m_TracerVericesRAM
		//glm::dvec3 tracerPosition = m_Tracers[i].position();
		//size_t iterationCounter = 0;
	}
	
}

void VortonSim::solveBoundaryConditions()
{
    static constexpr double vortonInfluencedRadiusFactor = 1.2;

    for (auto& sphere : m_RigidBodySim.spheres()) {
        for (auto& vorton : m_Vortons) {
            const auto vortonInfluencedRadius = vorton.radius() * vortonInfluencedRadiusFactor;
            const auto sphereToVortonVector = vorton.position() - sphere.position();
            const double sphereToVortonDistance = glm::length(sphereToVortonVector);
            if (sphereToVortonDistance < (sphere.radius() + vortonInfluencedRadius)) {
                solveVortonRigidBodySphereCollision(vorton, sphere, sphereToVortonVector, sphereToVortonDistance);
            }
        }

        for (auto& tracer : m_Tracers) {
            const auto sphereToTracerVector = tracer.position() - sphere.position();
            const auto sphereToTracerDistance = glm::length(sphereToTracerVector);
            if (sphereToTracerDistance < tracer.radius() + sphere.radius()) {
                solveTracerRigidBodySphereCollision(tracer, sphere, sphereToTracerVector, sphereToTracerDistance);
            }
        }
    }
}

void VortonSim::solveVortonRigidBodySphereCollision(Vorton& vorton, RigidBodySphere& sphere, const glm::dvec3& sphereToVortonVector, const double sphereToVortonDistance) const
{
    const auto sphereToVortonVectorNormalized = sphereToVortonVector / sphereToVortonDistance;
    const auto sphereContactPointLocal = sphereToVortonVectorNormalized * sphere.radius();
    const auto sphereContactPointWorld = sphereContactPointLocal + sphere.position();
    const auto sphereLinearRotationVelocityAtContactPoint = glm::cross(sphere.angularVelocity(), sphereContactPointLocal);
    const auto sphereLinearVelocityAtContactPoint = sphere.velocity() + sphereLinearRotationVelocityAtContactPoint;

    const auto velocityFluidAtContactPoint = m_VelocityGridPtr->interpolate(sphereContactPointWorld);
    const auto velocityFluidWithoutInvolvedVorton = velocityFluidAtContactPoint - vorton.inducedVelocity(sphereContactPointWorld);
    const auto linearVelocitySphereToFluidDelta = velocityFluidWithoutInvolvedVorton - sphereLinearVelocityAtContactPoint;

    const auto surfaceNormal = sphereToVortonVectorNormalized;
    const auto deltaVelocityDirection = glm::normalize(linearVelocitySphereToFluidDelta);
    const auto newVorticityDirection = glm::cross(surfaceNormal, deltaVelocityDirection);
    const auto bendDirection = glm::normalize(glm::cross(newVorticityDirection, deltaVelocityDirection));

    const auto vortonToContactPointDistance = sphereToVortonDistance - sphere.radius();
    const auto newVortonToContactPointDistance = vortonToContactPointDistance < vorton.radius() ? vorton.radius() : vortonToContactPointDistance;

    const auto newVortonPosition = sphereContactPointWorld - (bendDirection * newVortonToContactPointDistance);
    vorton.position(newVortonPosition);

    const auto previousVorticity = vorton.vorticity();
    vorton.vorticityByVelocity(-linearVelocitySphereToFluidDelta, sphereContactPointWorld);

    //delay shedding
    constexpr double sheddingGain = 0.9;
    vorton.vorticity(vorton.vorticity() * sheddingGain + (1 - sheddingGain)*previousVorticity);

    //transfer angular momentum
    const auto deltaVorticity = vorton.vorticity() - previousVorticity;

    //accroding to inertia of cylinder iz = mr^2/2 and vorticity = 2 * angular velocity (https://de.wikipedia.org/wiki/Wirbelst%C3%A4rke)
    //follows I_vor = mr^2 which is also equal to moment of inertia of point mass
    //const double momentOfInertiaVorton = /*vorton.radius() * vorton.radius()*/ 3.0 * vorton.mass();
    //sphere.addAngularMomentum(momentOfInertiaVorton * deltaVorticity);

    //transfer linear momentum
    //const auto deltaVelocitySphereToVorton = vorton.velocity() - sphereLinearVelocityAtContactPoint;
    //sphere.addMomentum(deltaVelocitySphereToVorton * vorton.mass());
    sphere.addMomentum((velocityFluidAtContactPoint -sphereLinearVelocityAtContactPoint) * vorton.mass(), sphereContactPointWorld);

    vorton.velocity(sphereLinearVelocityAtContactPoint);
}

void VortonSim::solveTracerRigidBodySphereCollision(Tracer& tracer, RigidBodySphere& sphere, const glm::dvec3& sphereToTracerVector,
    const double sphereToTracerDistance) const
{
    const double distanceRescale = (tracer.radius() + sphere.radius()) * (1 + std::numeric_limits<float>::epsilon()) / sphereToTracerDistance;
    const auto newPositionDelta = distanceRescale * sphereToTracerVector;
    tracer.position(sphere.position() + newPositionDelta);

    const auto sphereLinearRotationVelocity = glm::cross(sphere.angularVelocity(), newPositionDelta);
    const auto sphereLinearVelocityAtContactPoint = sphere.velocity() + sphereLinearRotationVelocity;
    const auto velocityTracerDelta = tracer.velocity() - sphereLinearVelocityAtContactPoint;

    //sphere.addMomentum(velocityTracerDelta * tracer.mass());

    tracer.velocity(sphereLinearVelocityAtContactPoint);
}


std::pair<glm::dvec3, glm::dvec3> VortonSim::velocityGridDimensions()
{
	//find min corner and max corner
	glm::dvec3 minCorner = glm::dvec3(FLT_MAX);
	glm::dvec3 maxCorner = glm::dvec3(-FLT_MAX);
    double maxRadius = 0.0;

	for (auto & tracer : m_Tracers) {
		minCorner = fsmath::allMin(tracer.position(), minCorner);
		maxCorner = fsmath::allMax(tracer.position(), maxCorner);
        maxRadius = std::max(maxRadius, tracer.radius());
	}

	for (auto & vorton : m_Vortons) {
		minCorner = fsmath::allMin(vorton.position(), minCorner);
		maxCorner = fsmath::allMax(vorton.position(), maxCorner);
        maxRadius = std::max(maxRadius, vorton.radius());
	}

    for (auto& sphere : m_RigidBodySim.spheres()) {
        minCorner = fsmath::allMin(sphere.position(), minCorner);
        maxCorner = fsmath::allMax(sphere.position(), maxCorner);
        maxRadius = std::max(maxRadius, sphere.radius());
    }

    minCorner -= glm::dvec3(maxRadius);
    maxCorner += glm::dvec3(maxRadius);

	glm::dvec3 extentAdjustment = static_cast<double>(FLT_EPSILON) * (maxCorner - minCorner);
	minCorner -= extentAdjustment;
	maxCorner += extentAdjustment;

	//return dimensions as minCorner, maxCorner
	return std::make_pair(minCorner, maxCorner);
}