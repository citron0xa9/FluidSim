#pragma once

#include "LightSource.h"
#include <glm/vec3.hpp>

class SunLightSource : public LightSource
{
public:
	SunLightSource(GLfloat intensity, glm::vec3 direction);
	virtual ~SunLightSource();

	virtual void loadIntoProgram(Program& program) const override;
	virtual LightSource* clone() const override;
	
private:
	glm::vec3 m_Direction;
};

