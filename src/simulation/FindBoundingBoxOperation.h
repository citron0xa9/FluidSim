#pragma once

#include "ParticleOperation.h"

#include "../util/fsmath.h"
#include "RigidBodySim.h"

#include <vector>
#include <functional>

class FindBoundingBoxOperation : public ParticleOperation
{
public:
    FindBoundingBoxOperation(ParticleSystem& parent,
        const std::vector<std::reference_wrapper<ParticleSystem>>& respectedParticleSystems,
        const std::vector<std::reference_wrapper<RigidBodySim>>& respectedRigidBodySims);

    virtual void process(const double stepSecondsPassed, const double secondsPassedTotal) override;

    const fsmath::BoundingBox& currentBoundingBox() const;

private:
    const std::vector<std::reference_wrapper<ParticleSystem>> m_RespectedParticleSystems;
    const std::vector<std::reference_wrapper<RigidBodySim>> m_RespectedRigidBodySims;

    fsmath::BoundingBox m_CurrentBoundingBox;

    glm::dvec3 calculateMaxCorner() const;
    void calculateBoundingBox();
    void resetBoundingBox();
    void updateBoundingBox(std::reference_wrapper<ParticleSystem> particleSystem);
    void updateBoundingBox(std::reference_wrapper<RigidBodySim> rigidBodySim);
    void updateBoundingBox(const glm::dvec3& newMinCorner, const glm::dvec3& newMaxCorner);
    void expandBoundingBox();
};

