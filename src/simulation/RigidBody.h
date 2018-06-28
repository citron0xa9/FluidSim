#pragma once

#include "../Object.h"

#include <glm/mat3x3.hpp>


class RigidBody : public Object {
    public:

        RigidBody(const glm::dvec3& initialPosition, const glm::dvec3& initialVelocity, const double mass);
        virtual ~RigidBody();

        glm::dvec3 velocity() const;
        glm::dvec3 angularVelocity() const;

        void applyForce(const glm::dvec3& force, const glm::dvec3& applyPosition);
        void applyTorque(const glm::dvec3& torque);

        void addMomentum(const glm::dvec3& momentum, const glm::dvec3& addLocation);
        void addMomentum(const glm::dvec3& momentum);
        void addAngularMomentum(const glm::dvec3& momentum);

        const glm::dvec3& momentum() const;
        const glm::dvec3& angularMomentum() const;
        const glm::dvec3& force() const;
        const glm::dvec3& torque() const;

        double massInverse() const;

        void update(const double secondsPassed);

        virtual Object* copy() const override;

    protected:
        glm::dmat3x3 m_InertiaInverse;

    private:
        double m_MassInverse;

        glm::dvec3 m_Velocity;
        glm::dvec3 m_AngularVelocity;
        glm::dvec3 m_Force;
        glm::dvec3 m_Torque;
        glm::dvec3 m_Momentum;
        glm::dvec3 m_AngularMomentum;
        glm::dvec3 m_Orientation;
};

