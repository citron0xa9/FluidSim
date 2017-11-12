#pragma once

#include "Object.h"

class Tracer : public Object {
    public:
        Tracer(const double initialRadius = 0.0, const double initialMass = 0.0);
        virtual ~Tracer();

        double radius() const;
        double mass() const;

        void mass(const double newMass);
        void radius(const double newRadius);

        void velocity(const glm::dvec3& velocity);
        const glm::dvec3& velocity() const;

        virtual Object* copy() const override;

    private:
        double m_Radius;
        double m_Mass;
        glm::dvec3 m_Velocity;
};

