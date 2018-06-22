#pragma once

#include "ParticleOperation.h"

#include "fsmath.h"

#include <vector>

class FindBoundingBoxOperation : public ParticleOperation
{
public:
    FindBoundingBoxOperation(ParticleSystem& parent, const std::vector<std::reference_wrapper<ParticleSystem>>& respectedParticleSystems);

    virtual void process(const double stepSecondsPassed, const double secondsPassedTotal) override;

    const fsmath::BoundingBox& currentBoundingBox() const;

private:
    const std::vector<std::reference_wrapper<ParticleSystem>> m_RespectedParticleSystems;

    fsmath::BoundingBox m_CurrentBoundingBox;

    void calculateBoundingBox();
    void resetBoundingBox();
    void updateBoundingBox(std::reference_wrapper<ParticleSystem> particleSystem);
    void expandBoundingBox();
};

