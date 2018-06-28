#pragma once

#include "../Object.h"

#include <chrono>

class Particle : public Object
{
public:
    Particle(const double birthTimeSeconds, const double initialRadius = 0.0, const double initialMass = 0.0);

    double radius() const;
    double mass() const;

    void velocity(const glm::dvec3& newVelocity);
    void addVelocity(const glm::dvec3& velocityToAdd);
    glm::dvec3 velocity() const;

    double birthTimeSeconds() const;

    virtual Particle* copy() const override;
private:
    double m_Radius;
    double m_Mass;
    glm::dvec3 m_Velocity;

    const double m_BirthTimeSeconds;
};

