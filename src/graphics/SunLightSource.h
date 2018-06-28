#pragma once

#include "LightSource.h"
#include <glm/vec3.hpp>

class SunLightSource : public LightSource
{
public:
	SunLightSource(Scene &scene, GLfloat intensity, glm::vec3 direction);
	virtual ~SunLightSource();

	virtual void loadIntoProgram(Program& program) const override;
	virtual Object* copy() const override;
	
private:
	glm::vec3 m_Direction;
};

