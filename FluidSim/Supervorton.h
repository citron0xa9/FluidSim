#pragma once

#include "Vorton.h"
#include <vector>

class Supervorton : public Vorton
{
public:
	Supervorton();
	Supervorton(const std::vector<const Vorton&> &containedVortons);
	virtual ~Supervorton();

	void addContainedVorton(const Vorton &vorton);

	std::vector<const Vorton&>& getContainedVortons();

private:
	Vorton calculateSupervorton(const std::vector<const Vorton&> &containedVortons);

	std::vector<const Vorton&> m_ContainedVortons;
	float m_VorticityMagnitudeSum;
};