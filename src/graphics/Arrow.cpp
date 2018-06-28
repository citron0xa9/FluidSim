
#include "Arrow.h"

#include <glm/gtx/string_cast.hpp>
#include "../util/Log.h"

Arrow::Arrow(const glm::dvec3 &velocity, float angle)
	: m_Velocity{velocity}, m_Angle{angle}, m_IsHighlighted{false}
{
}


Arrow::~Arrow()
{

}

Object * Arrow::copy() const
{
	return new Arrow{ *this };
}

void Arrow::printInfo() const
{
	std::string infoString;
	infoString += "Position: " + glm::to_string(position()) + "\n";
	infoString += "Velocity: " + glm::to_string(m_Velocity) + "\n";
	infoString += "VelocityNormalized: " + glm::to_string(m_VelocityNormalized) + "\n";
	infoString += "Angle: " + std::to_string(glm::degrees(m_Angle)) + "\n";
	INFO(infoString);
}

void Arrow::velocity(const glm::dvec3 & velocity)
{
	m_Velocity = velocity;
}

void Arrow::velocityNormalized(const glm::dvec3 & velocity)
{
	m_VelocityNormalized = velocity;
}

void Arrow::angle(float angle)
{
	m_Angle = angle;
}

void Arrow::toggleHighlight()
{
	m_IsHighlighted = !m_IsHighlighted;
}

bool Arrow::isHighlighted() const
{
	return m_IsHighlighted;
}
