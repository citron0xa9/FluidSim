#pragma once

#include <string>
#include <GL/glew.h>
#include "../Object.h"

class Program;
class Scene;

enum class LightSourceType {
	SUNLIGHT
};

struct ShaderLightSourceVariable {
	std::string name;
	LightSourceType type;
	ShaderLightSourceVariable(const char *cstrName, LightSourceType type) : name( cstrName ), type( type ) {}
};

class LightSource : public Object
{
public:
	LightSource(Scene &scene, GLfloat intensity);
	virtual ~LightSource();
	virtual void loadIntoProgram(Program &program) const = 0;

	virtual void registerSceneHooks(Scene &scene) override;

protected:
	GLfloat m_Intensity;
};

