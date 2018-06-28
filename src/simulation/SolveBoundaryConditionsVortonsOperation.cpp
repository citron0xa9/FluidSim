
#include "SolveBoundaryConditionsVortonsOperation.h"

#include "Vorton.h"

#include <glm/geometric.hpp>


SolveBoundaryConditionsVortonsOperation::SolveBoundaryConditionsVortonsOperation(
    ParticleSystem& parent,
    RigidBodySim& rigidBodySim,
    const get_grid_function_t& getFluidVelocityGridFunction)
    : SolveBoundaryConditionsOperation{parent, rigidBodySim}, m_GetFluidVelocityGridFunction{getFluidVelocityGridFunction}
{
}

bool SolveBoundaryConditionsVortonsOperation::isCollision(const RigidBodySphere & sphere, const Particle& particle) const
{
    static constexpr double vortonInfluencedRadiusFactor = 1.2;

    const auto vortonInfluencedRadius = particle.radius() * vortonInfluencedRadiusFactor;
    const auto sphereToVortonVector = particle.position() - sphere.position();
    const double sphereToVortonDistance = glm::length(sphereToVortonVector);
    return (sphereToVortonDistance < (sphere.radius() + vortonInfluencedRadius));
}

void SolveBoundaryConditionsVortonsOperation::solveParticleRigidBodySphereCollision(RigidBodySphere & sphere, Particle & particle)
{
    const auto& velocityGridPtr = m_GetFluidVelocityGridFunction();
    assert(velocityGridPtr);
    Vorton& vorton = static_cast<Vorton&>(particle);

    const auto sphereToVortonVector = vorton.position() - sphere.position();
    const double sphereToVortonDistance = glm::length(sphereToVortonVector);

    const auto sphereToVortonVectorNormalized = sphereToVortonVector / sphereToVortonDistance;
    const auto sphereContactPointLocal = sphereToVortonVectorNormalized * sphere.radius();
    const auto sphereContactPointWorld = sphereContactPointLocal + sphere.position();
    const auto sphereLinearRotationVelocityAtContactPoint = glm::cross(sphere.angularVelocity(), sphereContactPointLocal);
    const auto sphereLinearVelocityAtContactPoint = sphere.velocity() + sphereLinearRotationVelocityAtContactPoint;

    const auto velocityFluidAtContactPointTotal = vorton.velocity();
    const auto velocityFluidThroughVorticityAtContactPoint = velocityGridPtr->interpolate(sphereContactPointWorld);
    const auto velocityFluidThroughVorticityWithoutInvolvedVorton = velocityFluidThroughVorticityAtContactPoint - vorton.inducedVelocity(sphereContactPointWorld);
    const auto velocityFluidThroughExternalFactors = velocityFluidAtContactPointTotal - velocityFluidThroughVorticityAtContactPoint;
    const auto velocityFluidAtContactPointTotalWithoutInvolvedVorton = velocityFluidThroughExternalFactors + velocityFluidThroughVorticityWithoutInvolvedVorton;

    const auto linearVelocitySphereToFluidDelta = velocityFluidAtContactPointTotalWithoutInvolvedVorton - sphereLinearVelocityAtContactPoint;

    const auto surfaceNormal = sphereToVortonVectorNormalized;
    const auto deltaVelocityDirection = glm::normalize(linearVelocitySphereToFluidDelta);
    const auto newVorticityDirection = glm::cross(surfaceNormal, deltaVelocityDirection);
    const auto bendDirection = glm::normalize(glm::cross(newVorticityDirection, deltaVelocityDirection));

    const auto vortonToContactPointDistance = sphereToVortonDistance - sphere.radius();
    const auto newVortonToContactPointDistance = vortonToContactPointDistance < vorton.radius() ? vorton.radius() : vortonToContactPointDistance;

    const auto newVortonPosition = sphereContactPointWorld - (bendDirection * newVortonToContactPointDistance);
    vorton.position(newVortonPosition);

    const auto previousVorticity = vorton.vorticity();
    vorton.vorticityByVelocity(-linearVelocitySphereToFluidDelta, sphereContactPointWorld);

    //delay shedding
    constexpr double sheddingGain = 0.1;
    vorton.vorticity(vorton.vorticity() * sheddingGain + (1 - sheddingGain)*previousVorticity);

    //transfer angular momentum
    const auto deltaVorticity = vorton.vorticity() - previousVorticity;

    //accroding to inertia of cylinder iz = mr^2/2 and vorticity = 2 * angular velocity (https://de.wikipedia.org/wiki/Wirbelst%C3%A4rke)
    //follows I_vor = mr^2 which is also equal to moment of inertia of point mass
    //const double momentOfInertiaVorton = /*vorton.radius() * vorton.radius()*/ 3.0 * vorton.mass();
    //sphere.addAngularMomentum(momentOfInertiaVorton * deltaVorticity);

    //transfer linear momentum
    //const auto deltaVelocitySphereToVorton = vorton.velocity() - sphereLinearVelocityAtContactPoint;
    //sphere.addMomentum(deltaVelocitySphereToVorton * vorton.mass());
    sphere.addMomentum(linearVelocitySphereToFluidDelta * vorton.mass(), sphereContactPointWorld);

    vorton.velocity(sphereLinearVelocityAtContactPoint);
}
