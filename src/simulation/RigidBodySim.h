#pragma once

#include "../ActiveObject.h"
#include "RigidBodySphere.h"

class RigidBodySim : public ActiveObject
{
    public:
        RigidBodySim();
        ~RigidBodySim();
        RigidBodySim(const RigidBodySim& original);

        virtual void step(double secondsPassed) override;

        std::vector<RigidBodySphere>& spheres();

        virtual Object* copy() const override;

        void simulating(const bool shouldSimulate);
        void simulationTimescale(double timescale);

    private:
        std::vector<RigidBodySphere> m_Spheres;

        bool m_IsSimulating;
        double m_SimulationTimescale;
};

