#include "SunLightSource.h"


SunLightSource::SunLightSource(GLfloat intensity, glm::vec3 direction) : LightSource{ intensity }, m_Direction{direction}
{
}


SunLightSource::~SunLightSource()
{
}
