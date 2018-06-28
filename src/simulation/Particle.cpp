
#include "Particle.h"


Particle::Particle(const double birthTimeSeconds, const double initialRadius = 0.0, const double initialMass = 0.0)
    : m_BirthTimeSeconds{birthTimeSeconds}, m_Radius{initialRadius}, m_Mass{initialMass}, m_Velocity{0.0}
{
}

double Particle::radius() const
{
    return m_Radius;
}

double Particle::mass() const
{
    return m_Mass;
}

void Particle::velocity(const glm::dvec3& newVelocity)
{
    m_Velocity = newVelocity;
}

void Particle::addVelocity(const glm::dvec3 & velocityToAdd)
{
    m_Velocity += velocityToAdd;
}

glm::dvec3 Particle::velocity() const
{
    return m_Velocity;
}

double Particle::birthTimeSeconds() const
{
    return m_BirthTimeSeconds;
}

Particle* Particle::copy() const
{
    return new Particle{ *this };
}
