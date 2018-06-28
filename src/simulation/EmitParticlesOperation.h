#pragma once

#include "ParticleOperation.h"
#include "Particle.h"

#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <random>

class EmitParticlesOperation : public ParticleOperation
{
public:
    using creation_function_t =
        std::function<std::unique_ptr<Particle>(const double birthTimeSeconds, const double initialRadius, const double initialMass)>;

    EmitParticlesOperation(
        ParticleSystem& parent,
        const size_t amountPerSecond,
        const glm::dvec3& baseVelocity,
        const glm::dvec3& velocitySpread,
        const glm::dvec3& basePosition,
        const glm::dvec3& positionSpread,
        const double initialRadius,
        const double initialMass,
        const creation_function_t& creationFunction);
    EmitParticlesOperation(const EmitParticlesOperation& original) = delete;

    EmitParticlesOperation& operator=(const EmitParticlesOperation& original) = delete;

    virtual void process(const double stepSecondsPassed, const double secondsPassedTotal) override;

    void emitParticle(const double stepSecondsPassed, const double secondsPassedTotal);

private:
    const size_t m_AmountPerSecond;
    double m_LastEmissionRemainder;

    const glm::dvec3 m_BaseVelocity;
    const glm::dvec3 m_VelocitySpread;
    const glm::dvec3 m_BasePosition;
    const glm::dvec3 m_PositionSpread;

    const double m_InitialRadius;
    const double m_InitialMass;

    creation_function_t m_CreationFunction;

    std::uniform_real_distribution<double> m_MinusOneToOneDistribution;

    static std::default_random_engine s_RandomGenerator;

    glm::dvec3 generateRandomVector() const;
};

