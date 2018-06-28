
#include "SolveBoundaryConditionsOperation.h"

#include "ParticleSystem.h"

#include <glm/geometric.hpp>


SolveBoundaryConditionsOperation::SolveBoundaryConditionsOperation(ParticleSystem& parent, RigidBodySim& rigidBodySim)
    : ParticleOperation{parent}, m_RigidBodySim{rigidBodySim}
{
}

void SolveBoundaryConditionsOperation::process(const double stepSecondsPassed, const double secondsPassedTotal)
{
    for (auto& sphere : m_RigidBodySim.spheres()) {
        for (auto& particlePtr : m_Parent.particlePtrs()) {
            if (isCollision(sphere, *particlePtr)) {
                solveParticleRigidBodySphereCollision(sphere, *particlePtr);
            }
        }
    }
}

bool SolveBoundaryConditionsOperation::isCollision(const RigidBodySphere & sphere, const Particle & particle) const
{
    const auto sphereToParticleVector = particle.position() - sphere.position();
    const auto sphereToParticleDistance = glm::length(sphereToParticleVector);
    return (sphereToParticleDistance < particle.radius() + sphere.radius());
}

void SolveBoundaryConditionsOperation::solveParticleRigidBodySphereCollision(RigidBodySphere & sphere, Particle & particle)
{
    const auto sphereToParticleVector = particle.position() - sphere.position();
    const auto sphereToParticleDistance = glm::length(sphereToParticleVector);
    const double distanceRescale = (particle.radius() + sphere.radius()) * (1 + std::numeric_limits<float>::epsilon()) / sphereToParticleDistance;
    const auto newPositionDelta = distanceRescale * sphereToParticleVector;
    particle.position(sphere.position() + newPositionDelta);

    const auto sphereLinearRotationVelocity = glm::cross(sphere.angularVelocity(), newPositionDelta);
    const auto sphereLinearVelocityAtContactPoint = sphere.velocity() + sphereLinearRotationVelocity;
    const auto velocityParticleDelta = particle.velocity() - sphereLinearVelocityAtContactPoint;

    particle.velocity(sphereLinearVelocityAtContactPoint);
}
