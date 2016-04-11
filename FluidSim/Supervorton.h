#pragma once

#include "Vorton.h"
#include <vector>

class Supervorton : public Vorton
{
public:
	Supervorton();
	Supervorton(const std::vector<Vorton&> &containedVortons);
	virtual ~Supervorton();

	void addContainedVorton(Vorton &vorton);

	std::vector<Vorton&>& getContainedVortons();

private:
	Vorton calculateSupervorton(const std::vector<Vorton&> &containedVortons);

	std::vector<Vorton&> m_ContainedVortons;
	float m_VorticityMagnitudeSum;
};