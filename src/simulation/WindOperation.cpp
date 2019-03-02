
#include "WindOperation.h"

#include "ParticleSystem.h"

WindOperation::WindOperation(ParticleSystem& parent, const glm::dvec3& windVelocity, const double gain)
    : ParticleOperation{parent},
    m_WindVelocity{windVelocity},
    m_Gain{gain}
{

}

void WindOperation::process(const double stepSecondsPassed, const double secondsPassedTotal)
{
    for (auto& particlePtr : m_Parent.particlePtrs()) {
        const auto particleToWindVelocityDelta = m_WindVelocity - particlePtr->velocity();
        particlePtr->addVelocity(m_Gain * particleToWindVelocityDelta);
    }
}
