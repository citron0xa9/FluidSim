#pragma once

#include "Vorton.h"
#include <vector>

class Supervorton : public Vorton
{
public:
	Supervorton(const double birthTimeSeconds, double radius, const double mass);

	void addContainedVorton(Vorton &vorton);

	std::vector<Vorton*>& containedVortonPtrs();
	void containedVortonPtrs(const std::vector<Vorton*> &containedVortons);

private:
	void calculateQuantitiesFromContainedVortons(const std::vector<Vorton*> &containedVortonPtrs);

	std::vector<Vorton*> m_ContainedVortonPtrs;
	double m_VorticityMagnitudeSum;
};