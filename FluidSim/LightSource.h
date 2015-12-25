#pragma once

#include <string>
#include "Program.h"
#include <gl/glew.h>

enum class LightSourceType {
	SUNLIGHT
};

struct ShaderLightSourceVariable {
	std::string name;
	LightSourceType type;
	ShaderLightSourceVariable(const char *cstrName, LightSourceType t) : name{ cstrName }, type{ t } {}
};

class LightSource
{
public:
	LightSource(GLfloat intensity);
	virtual ~LightSource();
	virtual void loadIntoProgram(Program &program) const = 0;

protected:
	GLfloat m_Intensity;
};

