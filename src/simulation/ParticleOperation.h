#pragma once

class ParticleSystem;

class ParticleOperation
{
public:
    ParticleOperation(ParticleSystem& parent);

    virtual void process(const double stepSecondsPassed, const double secondsPassedTotal) = 0;

protected:
    ParticleSystem& m_Parent;
};

