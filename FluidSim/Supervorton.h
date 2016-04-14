#pragma once

#include "Vorton.h"
#include <vector>

class Supervorton : public Vorton
{
public:
	Supervorton(const Supervorton &superVorton);
	Supervorton(const Vorton &vorton);
	Supervorton(const std::vector<Vorton*> &containedVortonPtrs);
	virtual ~Supervorton();

	void addContainedVorton(Vorton &vorton);

	std::vector<Vorton*>& getContainedVortonPtrs();
	void setContainedVortonPtrs(const std::vector<Vorton*> &containedVortons);

private:
	Vorton calculateSupervorton(const std::vector<Vorton*> &containedVortonPtrs);

	std::vector<Vorton*> m_ContainedVortonPtrs;
	float m_VorticityMagnitudeSum;
};