#pragma once

#include "SolveBoundaryConditionsOperation.h"

#include "UniformGrid.h"

#include <memory>

class SolveBoundaryConditionsVortonsOperation : public SolveBoundaryConditionsOperation
{
public:
    using get_grid_function_t = std::function<const std::unique_ptr<UniformGrid<glm::dvec3>>&()>;
    SolveBoundaryConditionsVortonsOperation(ParticleSystem& parent, RigidBodySim& rigidBodySim, const get_grid_function_t& getFluidVelocityGridFunction);

protected:
    virtual bool isCollision(const RigidBodySphere& sphere, const Particle& particle) const override;
    virtual void solveParticleRigidBodySphereCollision(RigidBodySphere& sphere, Particle& particle) override;

private:
    get_grid_function_t m_GetFluidVelocityGridFunction;
};

