#pragma once

#include <string>
#include <gl/glew.h>
#include "Object.h"
#include "Scene.h"

class Program;

enum class LightSourceType {
	SUNLIGHT
};

struct ShaderLightSourceVariable {
	std::string name;
	LightSourceType type;
	ShaderLightSourceVariable(const char *cstrName, LightSourceType t) : name( cstrName ), type( t ) {}
};

class LightSource : public Object
{
public:
	LightSource(Scene &scene, GLfloat intensity);
	virtual ~LightSource();
	virtual void loadIntoProgram(Program &program) const = 0;

	virtual void registerContainerObjectHooks() override;
	void registerSceneHooks(Scene &scene);

protected:
	GLfloat m_Intensity;
};

