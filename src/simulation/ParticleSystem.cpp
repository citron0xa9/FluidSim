
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem()
    : m_CreationTime{std::chrono::steady_clock::now()}
{
}

void ParticleSystem::step(double secondsPassed)
{
    using duration_seconds_t = std::chrono::duration<double>;
    const double totalSecondsPassed = std::chrono::duration_cast<duration_seconds_t>(std::chrono::steady_clock::now() - m_CreationTime).count();
    for (auto& operationPtr : m_ParticleOperationPtrs) {
        operationPtr->process(secondsPassed, totalSecondsPassed);
    }
}

void ParticleSystem::addParticle(std::unique_ptr<Particle>&& particlePtr)
{
    std::unique_lock<std::shared_mutex> particleVectorLock{ m_ParticleVectorMutex };
    m_ParticlePtrs.push_back(std::move(particlePtr));
}

void ParticleSystem::addParticleOperation(std::unique_ptr<ParticleOperation>&& operationPtr)
{
    m_ParticleOperationPtrs.push_back(std::move(operationPtr));
}

std::vector<std::unique_ptr<Particle>>& ParticleSystem::particlePtrs()
{
    return m_ParticlePtrs;
}

const std::vector<std::unique_ptr<Particle>>& ParticleSystem::particlePtrs() const
{
    return m_ParticlePtrs;
}

std::shared_mutex & ParticleSystem::particleVectorMutex() const
{
    return m_ParticleVectorMutex;
}

ParticleSystem* ParticleSystem::copy() const
{
    throw std::runtime_error("Cannot copy a particle system");
}
