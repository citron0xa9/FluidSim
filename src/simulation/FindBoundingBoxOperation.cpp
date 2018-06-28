
#include "FindBoundingBoxOperation.h"

#include "ParticleSystem.h"

#include <cassert>

FindBoundingBoxOperation::FindBoundingBoxOperation(ParticleSystem& parent, const std::vector<std::reference_wrapper<ParticleSystem>>& respectedParticleSystems)
    : ParticleOperation{parent}, m_RespectedParticleSystems{respectedParticleSystems}
{
    assert(!respectedParticleSystems.empty());
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

void FindBoundingBoxOperation::calculateBoundingBox()
{
    resetBoundingBox();
    for (auto particleSystem : m_RespectedParticleSystems) {
        updateBoundingBox(particleSystem);
    }
    expandBoundingBox(); //slightly expand to allow rounding errors etc.
}

void FindBoundingBoxOperation::resetBoundingBox()
{
    m_CurrentBoundingBox.m_MinCorner = glm::dvec3{ std::numeric_limits<double>::max() };
    m_CurrentBoundingBox.m_Extent = glm::dvec3{ 0.0 };
}

void FindBoundingBoxOperation::updateBoundingBox(std::reference_wrapper<ParticleSystem> particleSystem)
{
    glm::dvec3 minCorner = m_CurrentBoundingBox.m_MinCorner;
    glm::dvec3 maxCorner = minCorner + m_CurrentBoundingBox.m_Extent;

    for (auto& particlePtr : particleSystem.get().particlePtrs()) {
        minCorner = fsmath::allMin(minCorner, particlePtr->position());
        maxCorner = fsmath::allMax(maxCorner, particlePtr->position());
    }
    m_CurrentBoundingBox.m_Extent = (maxCorner - minCorner);
    m_CurrentBoundingBox.m_MinCorner = minCorner;
}

void FindBoundingBoxOperation::expandBoundingBox()
{
    glm::dvec3 adjustment = static_cast<double>(std::numeric_limits<float>::epsilon()) * m_CurrentBoundingBox.m_Extent; //use float epsilon for slightly larger expansion
    
    m_CurrentBoundingBox.m_MinCorner -= adjustment;
    m_CurrentBoundingBox.m_Extent += adjustment + adjustment;
}


