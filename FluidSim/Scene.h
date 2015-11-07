#pragma once

#include <vector>
#include "Object.h"
#include "LightSource.h"

class Scene
{
public:
	Scene();
	virtual ~Scene();

	void addObject(const Object &obj);
	void addLightSource(const LightSource &lightSource);

	void render();

private:
	std::vector<Object> m_Objects;
	std::vector<LightSource> m_LightSources;
};

