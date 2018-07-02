
#include "EmitParticlesOperation.h"

#include "ParticleSystem.h"
#include "../util/Log.h"

#include <glm/gtx/string_cast.hpp>

std::default_random_engine EmitParticlesOperation::s_RandomGenerator{
    static_cast<std::default_random_engine::result_type>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count())
};

EmitParticlesOperation::EmitParticlesOperation(
    ParticleSystem& parent,
    const size_t amountPerSecond,
    const glm::dvec3& baseVelocity,
    const glm::dvec3& velocitySpread,
    const glm::dvec3& basePosition,
    const glm::dvec3& positionSpread,
    const double initialRadius,
    const double initialMass,
    const creation_function_t& creationFunction)
    : ParticleOperation{parent},
    m_AmountPerSecond{ amountPerSecond }, m_LastEmissionRemainder{ 0.0 },
    m_BaseVelocity {baseVelocity}, m_VelocitySpread{ velocitySpread },
    m_BasePosition{basePosition}, m_PositionSpread{positionSpread},
    m_InitialRadius{ initialRadius },
    m_InitialMass{initialMass},
    m_CreationFunction{creationFunction},
    m_MinusOneToOneDistribution{-1.0, 1.0}
{
}

void EmitParticlesOperation::process(const double stepSecondsPassed, const double secondsPassedTotal)
{
    const double particleAmount = m_AmountPerSecond * stepSecondsPassed + m_LastEmissionRemainder;
    const size_t particleAmountTruncated = static_cast<size_t>(particleAmount);
    m_LastEmissionRemainder = particleAmount - particleAmountTruncated;

    for (size_t i = 0; i < particleAmountTruncated; ++i) {
        emitParticle(stepSecondsPassed, secondsPassedTotal);
    }
}

void EmitParticlesOperation::emitParticle(const double stepSecondsPassed, const double secondsPassedTotal)
{
    const auto particleVelocity = m_BaseVelocity + m_VelocitySpread * generateRandomVector();
    const auto particlePosition = m_BasePosition + m_PositionSpread * generateRandomVector();

    auto particlePtr = m_CreationFunction(secondsPassedTotal, m_InitialRadius, m_InitialMass);
    particlePtr->position(particlePosition);
    particlePtr->velocity(particleVelocity);
    m_Parent.addParticle(std::move(particlePtr));
}

glm::dvec3 EmitParticlesOperation::generateRandomVector()
{
    return glm::dvec3{
        m_MinusOneToOneDistribution(s_RandomGenerator),
        m_MinusOneToOneDistribution(s_RandomGenerator),
        m_MinusOneToOneDistribution(s_RandomGenerator)
    };
}
