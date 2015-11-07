#pragma once

#include <GL/glew.h>
#include <unordered_map>
#include <string>
#include "Shader.h"
#include "Material.h"

struct UniformNames {
	std::string ambientCoefficient;
	std::string diffuseCoefficient;
	std::string specularCoefficient;
	std::string specularExponent;
	UniformNames(const char *ambC, const char *diffC, const char *specC, const char *specExp) : ambientCoefficient{ ambC }, diffuseCoefficient{ diffC }, specularCoefficient{ specC },
		specularExponent{ specExp } {}
};

class Program
{
public:
	Program();
	virtual ~Program();

	void attachShader(Shader* shader);
	void detachAllShaders();

	void link();

	void use();
	static void unuse();

	void loadMaterial(const Material &material);
private:
	GLuint m_Id;
	static GLuint m_usedProgramId;
	std::unordered_map<GLuint,Shader*> m_AttachedShaders;

	static const UniformNames m_UNIFORM_NAMES;
};

