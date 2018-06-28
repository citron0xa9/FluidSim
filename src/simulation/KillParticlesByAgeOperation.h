#pragma once

#include "ParticleOperation.h"

class KillParticlesByAgeOperation : public ParticleOperation
{
public:
    KillParticlesByAgeOperation(ParticleSystem& parent, const double secondsUntilKill);

    virtual void process(const double stepSecondsPassed, const double secondsPassedTotal) override;

private:
    const double m_SecondsUntilKill;
};

