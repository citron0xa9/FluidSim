
#include "RigidBodySim.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>


RigidBodySim::RigidBodySim()
    : m_IsSimulating{true}, m_SimulationTimescale{1.0}
{
    m_Spheres.emplace_back(glm::dvec3(-3, 0, 0), glm::dvec3(1, 0, 0), 0.2, 0.2);
    m_Spheres.back().addAngularMomentum(glm::dvec3{0.0, 0.0, 0.0});
}


RigidBodySim::~RigidBodySim()
{
}

RigidBodySim::RigidBodySim(const RigidBodySim& original)
    : m_IsSimulating{original.m_IsSimulating}, m_SimulationTimescale{original.m_SimulationTimescale}, m_Spheres{original.m_Spheres}
{
}

void RigidBodySim::step(double secondsPassed)
{
    std::lock_guard<std::mutex> lock(m_InUpdateMutex);
    if (!m_IsSimulating) {
        return;
    }
    secondsPassed *= m_SimulationTimescale;

    for (auto& sphere : m_Spheres) {
        sphere.update(secondsPassed);
        std::cout << "--------------------" << std::endl;
        std::cout << "Updated Sphere:" << std::endl;
        std::cout << "Position: " << glm::to_string(sphere.position()) << "Mass: " << 1.0 / sphere.massInverse() << std::endl;
        std::cout << "Velocity: " << glm::to_string(sphere.velocity()) << "; Angular Velocity: " << glm::to_string(sphere.angularVelocity()) << std::endl;
        std::cout << "Linear Momentum: " << glm::to_string(sphere.momentum()) << "; Angular Momentum: " << glm::to_string(sphere.angularMomentum()) << std::endl;
        std::cout << "Force: " << glm::to_string(sphere.force()) << "Torque: " << glm::to_string(sphere.torque()) << std::endl;
        std::cout << std::endl;
    }
}

std::vector<RigidBodySphere>& RigidBodySim::spheres()
{
    return m_Spheres;
}

Object* RigidBodySim::copy() const
{
    return new RigidBodySim{ *this };
}

void RigidBodySim::simulating(const bool shouldSimulate)
{
    m_IsSimulating = shouldSimulate;
}

void RigidBodySim::simulationTimescale(double timescale)
{
    m_SimulationTimescale = timescale;
}

std::mutex & RigidBodySim::inUpdateMutex()
{
    return m_InUpdateMutex;
}
