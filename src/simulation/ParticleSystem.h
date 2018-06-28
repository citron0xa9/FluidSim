#pragma once

#include "../ActiveObject.h"
#include "Particle.h"
#include "ParticleOperation.h"

#include <vector>
#include <memory>
#include <chrono>

class ParticleSystem : public ActiveObject
{
public:
    ParticleSystem();
    ParticleSystem(const ParticleSystem& original) = delete;

    ParticleSystem& operator=(const ParticleSystem& original) = delete;

    virtual void step(double secondsPassed) override;

    void addParticle(std::unique_ptr<Particle>&& particlePtr);
    void addParticleOperation(std::unique_ptr<ParticleOperation>&& operationPtr);

    std::vector<std::unique_ptr<Particle>>& particlePtrs();
    const std::vector<std::unique_ptr<Particle>>& particlePtrs() const;

    virtual ParticleSystem* copy() const override;

private:
    std::vector<std::unique_ptr<Particle>> m_ParticlePtrs;
    std::vector<std::unique_ptr<ParticleOperation>> m_ParticleOperationPtrs;

    std::chrono::time_point<std::chrono::steady_clock> m_CreationTime;
};

