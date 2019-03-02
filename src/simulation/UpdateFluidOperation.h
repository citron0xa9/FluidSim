#pragma once

#include "ParticleOperation.h"

#include "FindBoundingBoxOperation.h"

#include "VortonOctHeap.h"
#include "UniformGrid.h"

#include <memory>

class UpdateFluidOperation : public ParticleOperation
{
public:
    UpdateFluidOperation(ParticleSystem& parent, const double viscosity,
        const std::vector<std::reference_wrapper<ParticleSystem>>& sizeRespectedParticleSystems,
        const std::vector<std::reference_wrapper<RigidBodySim>>& sizeRespectedRigidBodySims);

    virtual void process(const double stepSecondsPassed, const double secondsPassedTotal) override;

    const std::unique_ptr<UniformGrid<glm::dvec3>>& velocityGridPtr() const;
    const std::unique_ptr<VortonOctHeap>& vortonOctHeapPtr() const;

private:
    const double m_Viscosity;

    bool m_Simulating;

    std::unique_ptr<VortonOctHeap> m_VortonHeapPtr;
    std::unique_ptr<UniformGrid<glm::dvec3>> m_VelocityGridPtr;

    std::vector<std::reference_wrapper<Vorton>> m_CurrentVortons;

    FindBoundingBoxOperation m_BoundingBoxOperation;

    void retrieveCurrentVortons();

    void createOctHeap();

    void calculateVelocityGrid();
    glm::dvec3 calculateVelocity(const glm::dvec3 &position);

    void stretchAndTiltVortons(double seconds);

    void diffuseVorticityPSE(double seconds);
    void diffuseVorticityInside(double seconds, std::vector<Vorton*> &vortonPtrs);
    void diffuseVorticityBetween(double seconds, std::vector<Vorton*> &firstVortonPtrs, std::vector<Vorton*> &secondVortonPtrs);
    void diffuseVorticityBetween(double seconds, Vorton &first, Vorton &second);
};

