
#include "FindBoundingBoxOperation.h"

#include "ParticleSystem.h"

#include <cassert>

FindBoundingBoxOperation::FindBoundingBoxOperation(ParticleSystem& parent,
    const std::vector<std::reference_wrapper<ParticleSystem>>& respectedParticleSystems,
    const std::vector<std::reference_wrapper<RigidBodySim>>& respectedRigidBodySims)
    : ParticleOperation{parent},
    m_RespectedParticleSystems{respectedParticleSystems},
    m_RespectedRigidBodySims{respectedRigidBodySims}
{
    assert(!respectedParticleSystems.empty() || !respectedRigidBodySims.empty());
    calculateBoundingBox();
}

void FindBoundingBoxOperation::process(const double stepSecondsPassed, const double secondsPassedTotal)
{
    calculateBoundingBox();
}

const fsmath::BoundingBox & FindBoundingBoxOperation::currentBoundingBox() const
{
    return m_CurrentBoundingBox;
}

glm::dvec3 FindBoundingBoxOperation::calculateMaxCorner() const
{
    if (m_CurrentBoundingBox.m_Extent == glm::dvec3{ -std::numeric_limits<double>::infinity() }) {
        return glm::dvec3{ -std::numeric_limits<double>::infinity() };
    } else {
        return (m_CurrentBoundingBox.m_MinCorner + m_CurrentBoundingBox.m_Extent);
    }
}

void FindBoundingBoxOperation::calculateBoundingBox()
{
    resetBoundingBox();
    for (auto particleSystem : m_RespectedParticleSystems) {
        updateBoundingBox(particleSystem);
    }
    for (auto sim : m_RespectedRigidBodySims) {
        updateBoundingBox(sim);
    }
    expandBoundingBox(); //slightly expand to allow rounding errors etc.
}

void FindBoundingBoxOperation::resetBoundingBox()
{
    m_CurrentBoundingBox.m_MinCorner = glm::dvec3{ std::numeric_limits<double>::infinity() };
    m_CurrentBoundingBox.m_Extent = glm::dvec3{ -std::numeric_limits<double>::infinity() };
}

void FindBoundingBoxOperation::updateBoundingBox(std::reference_wrapper<ParticleSystem> particleSystem)
{
    if (particleSystem.get().particlePtrs().empty()) {
        return;
    }

    glm::dvec3 minCorner = m_CurrentBoundingBox.m_MinCorner;
    glm::dvec3 maxCorner = calculateMaxCorner();

    for (auto& particlePtr : particleSystem.get().particlePtrs()) {
        minCorner = fsmath::allMin(minCorner, particlePtr->position() - glm::dvec3{2*particlePtr->radius()});
        maxCorner = fsmath::allMax(maxCorner, particlePtr->position() + glm::dvec3{2*particlePtr->radius()});
    }

    updateBoundingBox(minCorner, maxCorner);
}

void FindBoundingBoxOperation::updateBoundingBox(std::reference_wrapper<RigidBodySim> rigidBodySim)
{
    if (rigidBodySim.get().spheres().empty()) {
        return;
    }

    glm::dvec3 minCorner = m_CurrentBoundingBox.m_MinCorner;
    glm::dvec3 maxCorner = calculateMaxCorner();

    for (auto& sphere : rigidBodySim.get().spheres()) {
        minCorner = fsmath::allMin(minCorner, sphere.position() - glm::dvec3{2*sphere.radius()});
        maxCorner = fsmath::allMax(maxCorner, sphere.position() + glm::dvec3{2*sphere.radius()});
    }

    updateBoundingBox(minCorner, maxCorner);
}

void FindBoundingBoxOperation::updateBoundingBox(const glm::dvec3& newMinCorner, const glm::dvec3& newMaxCorner)
{
    assert(newMinCorner != glm::dvec3{ std::numeric_limits<double>::infinity() });
    assert(newMaxCorner != glm::dvec3{ -std::numeric_limits<double>::infinity() });

    m_CurrentBoundingBox.m_Extent = fsmath::allMax(glm::dvec3{std::numeric_limits<float>::epsilon()},
                                                   (newMaxCorner - newMinCorner));
    m_CurrentBoundingBox.m_MinCorner = newMinCorner;
}

void FindBoundingBoxOperation::expandBoundingBox()
{
    glm::dvec3 adjustment = static_cast<double>(std::numeric_limits<float>::epsilon()) *
                            m_CurrentBoundingBox.m_Extent; //use float epsilon for slightly larger expansion
    
    m_CurrentBoundingBox.m_MinCorner -= adjustment;
    m_CurrentBoundingBox.m_Extent += adjustment + adjustment;
}



