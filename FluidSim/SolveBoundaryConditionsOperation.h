#pragma once

#include "ParticleOperation.h"
#include "Particle.h"

#include "RigidBodySim.h"

class SolveBoundaryConditionsOperation : public ParticleOperation
{
public:
    SolveBoundaryConditionsOperation(ParticleSystem& parent, RigidBodySim& rigidBodySim);

    virtual void process(const double stepSecondsPassed, const double secondsPassedTotal) override;

protected:
    virtual bool isCollision(const RigidBodySphere& sphere, const Particle& particle) const;
    virtual void solveParticleRigidBodySphereCollision(RigidBodySphere& sphere, Particle& particle);

private:
    RigidBodySim& m_RigidBodySim;
};

