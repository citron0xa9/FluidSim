
#include "Tracer.h"


Tracer::Tracer(const double initialRadius, const double initialMass)
    : m_Radius{ initialRadius }, m_Mass{ initialMass }, m_Velocity{0}
{
}


Tracer::~Tracer()
{
}

double Tracer::radius() const
{
    return m_Radius;
}

double Tracer::mass() const
{
    return m_Mass;
}

void Tracer::mass(const double newMass)
{
    m_Mass = newMass;
}

void Tracer::radius(const double newRadius)
{
    m_Radius = newRadius;
}

Object * Tracer::copy() const
{
    return new Tracer{ *this };
}

void Tracer::velocity(const glm::dvec3& velocity)
{
    m_Velocity = velocity;
}

const glm::dvec3& Tracer::velocity() const
{
    return m_Velocity;
}