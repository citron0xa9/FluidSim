#pragma once

#include <gl/glew.h>

class LightSource
{
public:
	LightSource(GLfloat intensity);
	virtual ~LightSource();

private:
	GLfloat m_Intensity;
};

