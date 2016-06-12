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

class VortonSim : public ActiveObject
{
public:
	
	VortonSim(double viscosity, double density, const VorticityDistribution &initialVorticity, double vorticityMagnitude);
	VortonSim(const VortonSim &original);
	~VortonSim();

	virtual void step(double secondsPassed) override;

	void update(double seconds);

	const std::vector<Object> &tracers() const;
	const std::vector<Vorton> &vortons() const;
	const std::shared_ptr<UniformGridGeometry> velocityGridPtr() const;

	void simulating(bool isSimulating);
	void simulationTimescale(double timescale);

	virtual Object* copy() const override;
	
	const VortonSim& operator=(const VortonSim& original) = delete;

	std::mutex &inUpdateMutex();

private:

	void initializeVortons(const VorticityDistribution &initialVorticity, double vorticityMagnitude);
	void initializeVortons(const VortonSim &original);
	
	void initializeTracers(const VorticityDistribution &initialVorticity);
	void initializeTracers(const VortonSim &original);

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

	std::pair<glm::dvec3, glm::dvec3> velocityGridDimensions();

	double m_Viscosity;
	double m_Density;
	std::vector<Vorton> m_Vortons;
	std::vector<Object> m_Tracers;

	std::shared_ptr<VortonOctHeap> m_VortonHeapPtr;
	std::shared_ptr<UniformGrid<glm::dvec3>> m_VelocityGridPtr;

	static const size_t m_VORTONS_PER_DIMENSION;
	static const size_t m_TRACERS_PER_DIMENSION;

	bool m_Simulating;
	double m_SimulationTimescale;

	std::mutex m_InUpdateMutex;

};

