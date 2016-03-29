#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "VorticityDistribution.h"
#include "VortonOctHeap.h"
#include "UniformGrid.h"
#include "Vorton.h"

class VortonSim
{
public:
	VortonSim(float viscosity, float density, const VorticityDistribution &initialVorticity, float vorticityMagnitude);
	~VortonSim();

	void Update(float seconds);

private:
	void CreateOctHeap();
	void CalculateVelocityGrid();
	glm::vec3 CalculateVelocity(const glm::vec3 &position);

	float m_Viscosity;
	float m_Density;
	std::vector<Vorton> m_Vortons;
	VortonOctHeap *m_VortonHeapPtr;
	UniformGrid<glm::vec3> *m_VelocityGridPtr;

	static const size_t m_VORTONS_PER_DIMENSION;
};

