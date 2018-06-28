
#include "AdvectParticlesOperation.h"

#include "ParticleSystem.h"


AdvectParticlesOperation::AdvectParticlesOperation(ParticleSystem& parent)
    : ParticleOperation{parent}
{
}

void AdvectParticlesOperation::process(const double stepSecondsPassed, const double secondsPassedTotal)
{
    for (auto& particlePtr : m_Parent.particlePtrs()) {
        const glm::dvec3 positionDelta = particlePtr->velocity() * stepSecondsPassed;
        particlePtr->translate(positionDelta);
    }
}
