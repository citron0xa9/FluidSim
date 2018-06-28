#pragma once

#include "ParticleOperation.h"

class AdvectParticlesOperation : public ParticleOperation
{
public:
    AdvectParticlesOperation(ParticleSystem& parent);

    virtual void process(const double stepSecondsPassed, const double secondsPassedTotal) override;
};

