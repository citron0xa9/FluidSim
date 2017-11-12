
#include "RigidBody.h"

#include <glm/geometric.hpp>

RigidBody::RigidBody(const glm::dvec3& initialPosition, const glm::dvec3& initialVelocity, const double mass)
    : Object{}, m_MassInverse {1.0 / mass}, m_Velocity{ initialVelocity }, m_AngularVelocity{0.0}, m_Force{0.0}, m_Torque{0.0}, m_Momentum{ initialVelocity * mass},
    m_AngularMomentum{ 0.0 }, m_InertiaInverse{ glm::dmat3x3{1.0} / mass }

{
    position(initialPosition);
}

RigidBody::~RigidBody()
{
}

glm::dvec3 RigidBody::velocity() const
{
    return m_Velocity;
}

glm::dvec3 RigidBody::angularVelocity() const
{
    return m_AngularVelocity;
}

void RigidBody::applyForce(const glm::dvec3& force, const glm::dvec3& applyPosition)
{
    m_Force += force;
    const auto applyPositionRelativeToBody = applyPosition - position();
    applyTorque(glm::cross(force, applyPositionRelativeToBody));
}

void RigidBody::applyTorque(const glm::dvec3& torque)
{
    m_Torque += torque;
}

void RigidBody::addMomentum(const glm::dvec3& momentum, const glm::dvec3& addLocation)
{
    //addMomentum(momentum);
    const auto locationToCenter = position() - addLocation;
    const auto locationToCenterNorm = glm::normalize(locationToCenter);

    const auto linearMomentumToCenter = glm::dot(momentum, locationToCenterNorm) * locationToCenterNorm;
    const auto linearMomentumTangent = momentum - linearMomentumToCenter;

    addMomentum(linearMomentumToCenter);
    addAngularMomentum(glm::cross(linearMomentumTangent, locationToCenter));
}

void RigidBody::addMomentum(const glm::dvec3& momentum)
{
    m_Momentum += momentum;
    m_Velocity = m_Momentum * m_MassInverse;
}

void RigidBody::addAngularMomentum(const glm::dvec3 & momentum)
{
    m_AngularMomentum += momentum;
    m_AngularVelocity = m_InertiaInverse * m_AngularMomentum;
}

const glm::dvec3 & RigidBody::momentum() const
{
    return m_Momentum;
}

const glm::dvec3 & RigidBody::angularMomentum() const
{
    return m_AngularMomentum;
}

const glm::dvec3 & RigidBody::force() const
{
    return m_Force;
}

const glm::dvec3 & RigidBody::torque() const
{
    return m_Torque;
}

double RigidBody::massInverse() const
{
    return m_MassInverse;
}

void RigidBody::update(const double secondsPassed)
{
    m_Momentum += m_Force * secondsPassed;
    m_AngularMomentum += m_Torque * secondsPassed;
    m_Velocity = m_Momentum * m_MassInverse;
    m_AngularVelocity = m_InertiaInverse * m_AngularMomentum;
    translate(m_Velocity * secondsPassed);

    m_Force = m_Torque = glm::dvec3(0.0);
}

Object * RigidBody::copy() const
{
    return new RigidBody{ *this };
}
