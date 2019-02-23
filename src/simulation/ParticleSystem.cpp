
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem()
    : m_CreationTime{std::chrono::steady_clock::now()}, mTotalSecondsPassed{0}
{
}

void ParticleSystem::step(double secondsPassed)
{
    mTotalSecondsPassed += secondsPassed;
    for (auto& operationPtr : m_ParticleOperationPtrs) {
        operationPtr->process(secondsPassed, mTotalSecondsPassed);
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
