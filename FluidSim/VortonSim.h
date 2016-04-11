#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "VorticityDistribution.h"
#include "VortonOctHeapElement.h"
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
	void StretchAndTiltVortons(float seconds);
	void DiffuseVorticityPSE(float seconds);
	void DiffuseVorticityInside(float seconds, std::vector<Vorton&> &vortons);
	void DiffuseVorticityBetween(float seconds, std::vector<Vorton&> &firstVortons, std::vector<Vorton&> &secondVortons);
	void DiffuseVorticityBetween(float seconds, Vorton &first, Vorton &second);

	float m_Viscosity;
	float m_Density;
	std::vector<Vorton> m_Vortons;
	VortonOctHeap *m_VortonHeapPtr;
	UniformGrid<glm::vec3> *m_VelocityGridPtr;

	static const size_t m_VORTONS_PER_DIMENSION;
};

