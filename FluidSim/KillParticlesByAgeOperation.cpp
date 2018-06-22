
#include "KillParticlesByAgeOperation.h"

#include "ParticleSystem.h"

#include <algorithm>

KillParticlesByAgeOperation::KillParticlesByAgeOperation(ParticleSystem& parent, const double secondsUntilKill)
    : ParticleOperation{parent}, m_SecondsUntilKill{secondsUntilKill}
{
}

void KillParticlesByAgeOperation::process(const double stepSecondsPassed, const double secondsPassedTotal)
{
    auto& particlePtrs = m_Parent.particlePtrs();
    auto newVectorEnd = std::remove_if(particlePtrs.begin(), particlePtrs.end(), [this, secondsPassedTotal](const std::unique_ptr<Particle>& particlePtr)
    {
        return ((particlePtr->birthTimeSeconds() + this->m_SecondsUntilKill) < secondsPassedTotal);
    });
    particlePtrs.erase(newVectorEnd, particlePtrs.end());
}
