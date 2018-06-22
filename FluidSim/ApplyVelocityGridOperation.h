#pragma once

#include "ParticleOperation.h"

#include "UniformGrid.h"

#include <functional>
#include <memory>

class ApplyVelocityGridOperation : public ParticleOperation
{
public:
    using get_grid_function_t = std::function<const std::unique_ptr<UniformGrid<glm::dvec3>>&()>;
    ApplyVelocityGridOperation(ParticleSystem& parent, const get_grid_function_t& getGridFunction);

    virtual void process(const double stepSecondsPassed, const double secondsPassedTotal) override;

private:
    get_grid_function_t m_GetGridFunction;
};

