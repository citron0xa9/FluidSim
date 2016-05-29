#pragma once

#include "Vorton.h"
#include <vector>

class Supervorton : public Vorton
{
public:
	Supervorton();
	virtual ~Supervorton();

	void addContainedVorton(Vorton &vorton);

	std::vector<Vorton*>& containedVortonPtrs();
	void containedVortonPtrs(const std::vector<Vorton*> &containedVortons);

private:
	Vorton calculateSupervorton(const std::vector<Vorton*> &containedVortonPtrs);

	std::vector<Vorton*> m_ContainedVortonPtrs;
	double m_VorticityMagnitudeSum;
};