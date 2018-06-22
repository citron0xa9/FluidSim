
#include "UpdateFluidOperation.h"

#include "ParticleSystem.h"

#include "VortonOctHeapElement.h"

UpdateFluidOperation::UpdateFluidOperation(ParticleSystem& parent, const double viscosity, const std::vector<std::reference_wrapper<ParticleSystem>>& sizeRespectedParticleSystems)
    : ParticleOperation{parent}, m_Viscosity{viscosity}, m_Simulating{true},
    m_BoundingBoxOperation{parent, sizeRespectedParticleSystems}
{
}

void UpdateFluidOperation::process(const double stepSecondsPassed, const double secondsPassedTotal)
{
    if (!m_Simulating) {
        return;
    }
    if (m_Parent.particlePtrs().empty()) {
        return;
    }

    m_BoundingBoxOperation.process(stepSecondsPassed, secondsPassedTotal);

    createOctHeap();
    calculateVelocityGrid();

    stretchAndTiltVortons(stepSecondsPassed);
    diffuseVorticityPSE(stepSecondsPassed);
}

const std::unique_ptr<UniformGrid<glm::dvec3>>& UpdateFluidOperation::velocityGridPtr() const
{
    return m_VelocityGridPtr;
}

const std::unique_ptr<VortonOctHeap>& UpdateFluidOperation::vortonOctHeapPtr() const
{
    return m_VortonHeapPtr;
}

void UpdateFluidOperation::retrieveCurrentVortons()
{
    m_CurrentVortons.clear();
    m_CurrentVortons.reserve(m_Parent.particlePtrs().size());
    for (auto& particlePtr : m_Parent.particlePtrs()) {
        m_CurrentVortons.push_back(std::ref(*static_cast<Vorton*>(particlePtr.get())));
    }
}

void UpdateFluidOperation::createOctHeap()
{
    retrieveCurrentVortons();
    m_VortonHeapPtr = std::make_unique<VortonOctHeap>(m_CurrentVortons);
}

void UpdateFluidOperation::calculateVelocityGrid()
{
    size_t numPoints = m_CurrentVortons.size();
    auto& boundingBox = m_BoundingBoxOperation.currentBoundingBox();

    auto velocityGridPtr = std::make_unique<UniformGrid<glm::dvec3>>(numPoints, boundingBox.m_MinCorner, boundingBox.m_MinCorner + boundingBox.m_Extent);

    //loop over every point in uniform grid
    size_t velocityGridOffset = 0;
    glm::dvec3 currentGridPosition = velocityGridPtr->minCorner();

    for (size_t zIndex = 0; zIndex < velocityGridPtr->pointsAmount().z; zIndex++) {
        for (size_t yIndex = 0; yIndex < velocityGridPtr->pointsAmount().y; yIndex++) {
            for (size_t xIndex = 0; xIndex < velocityGridPtr->pointsAmount().x; xIndex++) {
                velocityGridPtr->atOffset(velocityGridOffset) = calculateVelocity(currentGridPosition);
                velocityGridOffset++;
                currentGridPosition.x += velocityGridPtr->cellExtent().x;
            }
            currentGridPosition.x = velocityGridPtr->minCorner().x;
            currentGridPosition.y += velocityGridPtr->cellExtent().y;
        }
        currentGridPosition.y = velocityGridPtr->minCorner().y;
        currentGridPosition.z += velocityGridPtr->cellExtent().z;
    }
    m_VelocityGridPtr = std::move(velocityGridPtr);
}

glm::dvec3 UpdateFluidOperation::calculateVelocity(const glm::dvec3 & position)
{
    if (m_CurrentVortons.empty()) {
        return glm::dvec3(0);
    }
    assert(m_VortonHeapPtr != nullptr);
    return (m_VortonHeapPtr->root().calculateVelocity(position));
}

void UpdateFluidOperation::stretchAndTiltVortons(double seconds)
{
    static constexpr double FUDGE_FACTOR = 0.5; //for stability
    UniformGrid<glm::dmat3x3> velocityJacobianGrid(*m_VelocityGridPtr);
    fsmath::computeJacobian(velocityJacobianGrid, *m_VelocityGridPtr);

    for (auto vorton : m_CurrentVortons) {
        glm::dmat3x3 velocityJacobian = velocityJacobianGrid.interpolate(vorton.get().position());

        glm::dvec3 stretchTilt = vorton.get().vorticity() * velocityJacobian;
        glm::dvec3 newVorticity = vorton.get().vorticity() + FUDGE_FACTOR * stretchTilt * seconds;

        vorton.get().vorticity(newVorticity);
    }
}

void UpdateFluidOperation::diffuseVorticityPSE(double seconds)
{
    for (auto leafs = m_VortonHeapPtr->leafs(); leafs.first != leafs.second; leafs.first++) {
        Supervorton &currentSupervorton = leafs.first->supervorton();
        diffuseVorticityInside(seconds, currentSupervorton.containedVortonPtrs());

        for (auto neighborPtr : leafs.first->forwardNeighbors()) {
            Supervorton &neighborSupervorton = neighborPtr->supervorton();
            diffuseVorticityBetween(seconds, currentSupervorton.containedVortonPtrs(), neighborSupervorton.containedVortonPtrs());
        }

        //dissipate vorticity
        for (auto vortonPtr : currentSupervorton.containedVortonPtrs()) {
            vortonPtr->vorticity(vortonPtr->vorticity() - (vortonPtr->vorticity() * seconds * m_Viscosity));
        }
    }
}

void UpdateFluidOperation::diffuseVorticityInside(double seconds, std::vector<Vorton*>& vortonPtrs)
{
    for (auto outerIterator = vortonPtrs.begin(); outerIterator != vortonPtrs.end(); outerIterator++) {
        for (auto innerIterator = outerIterator + 1; innerIterator < vortonPtrs.end(); innerIterator++) {
            diffuseVorticityBetween(seconds, **outerIterator, **innerIterator);
        }
    }
}

void UpdateFluidOperation::diffuseVorticityBetween(double seconds, std::vector<Vorton*>& firstVortonPtrs, std::vector<Vorton*>& secondVortonPtrs)
{
    for (auto outerIterator = firstVortonPtrs.begin(); outerIterator != firstVortonPtrs.end(); outerIterator++) {
        for (auto innerIterator = secondVortonPtrs.begin(); innerIterator != secondVortonPtrs.end(); innerIterator++) {
            diffuseVorticityBetween(seconds, **outerIterator, **innerIterator);
        }
    }
}

void UpdateFluidOperation::diffuseVorticityBetween(double seconds, Vorton & first, Vorton & second)
{
    glm::dvec3 vorticityDifference = first.vorticity() - second.vorticity();
    glm::dvec3 exchange = 2 * m_Viscosity * seconds * vorticityDifference;
    first.vorticity(first.vorticity() - exchange);
    second.vorticity(second.vorticity() + exchange);
}