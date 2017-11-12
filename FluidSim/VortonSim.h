#pragma once

#include "ActiveObject.h"

#include <vector>
#include <memory>
#include <mutex>
#include <glm/vec3.hpp>
#include "VorticityDistribution.h"
#include "VortonOctHeapElement.h"
#include "VortonOctHeap.h"
#include "UniformGrid.h"
#include "Vorton.h"
#include "RigidBodySim.h"
#include "RigidBodySphere.h"

class VortonSim : public ActiveObject
{
public:
	
	VortonSim(RigidBodySim& rbSim, double viscosity, double density, const VorticityDistribution &initialVorticity, double vorticityMagnitude);
	VortonSim(RigidBodySim& rbSim, double viscosity, double density, const std::vector<const VorticityDistribution*> &initialVorticityPtrs, double vorticityMagnitude);
	VortonSim(const VortonSim &original);
	~VortonSim();

	virtual void step(double secondsPassed) override;

	void update(double seconds);

	const std::vector<Tracer> &tracers() const;
	const std::vector<Vorton> &vortons() const;
	const std::shared_ptr<UniformGrid<glm::dvec3>> velocityGridPtr() const;
	const std::shared_ptr<VortonOctHeap> vortonOctHeapPtr() const;

	void simulating(bool isSimulating);
	void simulationTimescale(double timescale);

	virtual Object* copy() const override;
	
	const VortonSim& operator=(const VortonSim& original) = delete;

	std::mutex &inUpdateMutex();

private:

	void initializeVortons(const VorticityDistribution &initialVorticity, double vorticityMagnitude);
	void initializeVortons(const VortonSim &original);
	
	void initializeTracers(const VorticityDistribution &initialVorticity);
	//void initializeTracers(const VortonSim &original);

    void initializeRigidBodies();
    void intializeRigidBodies(const VortonSim& original);

    void removeRigidBodyEmbeddedObjects();

	void createOctHeap();

	void calculateVelocityGrid();
	glm::dvec3 calculateVelocity(const glm::dvec3 &position);

	void stretchAndTiltVortons(double seconds);

	void diffuseVorticityPSE(double seconds);
	void diffuseVorticityInside(double seconds, std::vector<Vorton*> &vortonPtrs);
	void diffuseVorticityBetween(double seconds, std::vector<Vorton*> &firstVortonPtrs, std::vector<Vorton*> &secondVortonPtrs);
	void diffuseVorticityBetween(double seconds, Vorton &first, Vorton &second);

	void advectVortons(double secondsPassed);
	void advectTracers(double secondsPassed);
    void solveBoundaryConditions();
    void solveVortonRigidBodySphereCollision(Vorton& vorton, RigidBodySphere& sphere, const glm::dvec3& sphereToVortonVector, const double sphereToVortonDistance) const;
    void solveTracerRigidBodySphereCollision(Tracer& tracer, RigidBodySphere& sphere, const glm::dvec3& sphereToTracerVector, const double sphereToTracerDistance) const;

	std::pair<glm::dvec3, glm::dvec3> velocityGridDimensions();

	double m_Viscosity;
	double m_Density;
	std::vector<Vorton> m_Vortons;
	std::vector<Tracer> m_Tracers;
    RigidBodySim& m_RigidBodySim;

	std::shared_ptr<VortonOctHeap> m_VortonHeapPtr;
	std::shared_ptr<UniformGrid<glm::dvec3>> m_VelocityGridPtr;

	static constexpr size_t m_VORTONS_PER_DIMENSION = 16;
    static constexpr size_t m_VORTONS_TOTAL = m_VORTONS_PER_DIMENSION * m_VORTONS_PER_DIMENSION * m_VORTONS_PER_DIMENSION;
	static constexpr size_t m_TRACERS_PER_DIMENSION = 2 * m_VORTONS_PER_DIMENSION;
    static constexpr size_t m_TRACERS_TOTAL = m_TRACERS_PER_DIMENSION * m_VORTONS_PER_DIMENSION * m_TRACERS_PER_DIMENSION;

    double m_MassPerTracer;

	bool m_Simulating;
	double m_SimulationTimescale;

	std::mutex m_InUpdateMutex;

};

