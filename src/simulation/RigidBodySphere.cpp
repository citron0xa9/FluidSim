
#include "RigidBodySphere.h"


RigidBodySphere::RigidBodySphere(const glm::dvec3& initialPosition, const glm::dvec3& initialVelocity, const double mass, const double radius)
    : RigidBody{initialPosition, initialVelocity, mass}, m_Radius{radius}
{
    m_InertiaInverse = (glm::dmat3x3(1.0) * 5.0) / (2.0 * mass * radius * radius);
}


RigidBodySphere::~RigidBodySphere()
{
}

double RigidBodySphere::radius() const
{
    return m_Radius;
}

Object * RigidBodySphere::copy() const
{
    return new RigidBodySphere{ *this };
}
