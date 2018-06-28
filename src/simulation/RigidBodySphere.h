#pragma once

#include "RigidBody.h"

class RigidBodySphere : public RigidBody {
    public:
        RigidBodySphere(const glm::dvec3& initialPosition, const glm::dvec3& initialVelocity, const double mass, const double radius);
        virtual ~RigidBodySphere();

        double radius() const;

        virtual Object* copy() const override;

    private:
        const double m_Radius;
};

