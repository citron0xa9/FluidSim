#include "SunLightSource.h"
#include "Program.h"


SunLightSource::SunLightSource(Scene &scene, GLfloat intensity, glm::vec3 direction) : LightSource{ scene, intensity }, m_Direction{direction}
{
}


SunLightSource::~SunLightSource()
{
}

void SunLightSource::loadIntoProgram(Program &program) const
{
	auto freeSunLightSourceVariable = program.popFreeLightSourceVariable(LightSourceType::SUNLIGHT);

	GLint intensityUniformLocation = program.uniformLocation((freeSunLightSourceVariable.name + "_Intensity").c_str());
	GLint directionUniformLocation = program.uniformLocation((freeSunLightSourceVariable.name + "_Direction").c_str());

	if ((intensityUniformLocation == -1) || (directionUniformLocation == -1)) {
		throw std::logic_error("SunLightSource::loadIntoProgram: one uniform location of free variable is -1");
	}

	program.use();
	glUniform1f(intensityUniformLocation, m_Intensity);
	glUniform3fv(directionUniformLocation, 1, &m_Direction[0]);
}

Object* SunLightSource::copy() const
{
	return new SunLightSource(*this);
}