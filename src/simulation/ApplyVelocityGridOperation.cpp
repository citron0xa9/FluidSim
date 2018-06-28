
#include "ApplyVelocityGridOperation.h"

#include "ParticleSystem.h"


ApplyVelocityGridOperation::ApplyVelocityGridOperation(ParticleSystem& parent, const get_grid_function_t& getGridFunction)
    : ParticleOperation{parent}, m_GetGridFunction{getGridFunction}
{
}

void ApplyVelocityGridOperation::process(const double stepSecondsPassed, const double secondsPassedTotal)
{
    const auto& gridPtr = m_GetGridFunction();
    if (!gridPtr) {
        return;
    }
    for (auto& particlePtr : m_Parent.particlePtrs()) {
        particlePtr->addVelocity(gridPtr->interpolate(particlePtr->position()));
    }
}
