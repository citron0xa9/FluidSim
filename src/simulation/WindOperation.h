#pragma once

#include "ParticleOperation.h"

#include <glm/vec3.hpp>

class WindOperation : public ParticleOperation
{
public:
    WindOperation(ParticleSystem& parent, const glm::dvec3& windVelocity, const double gain = 1.0);

    virtual void process(const double stepSecondsPassed, const double secondsPassedTotal) override;

private:
    const glm::dvec3 m_WindVelocity;
    const double m_Gain;
};

