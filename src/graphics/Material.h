#pragma once

#include <glm/vec3.hpp>
#include <GL/glew.h>

class Program;


struct MaterialUniformLocations {
	GLint ambientCoefficient;
	GLint diffuseCoefficient;
	GLint specularCoefficient;
	GLint specularExponent;
};


class Material
{
public:
	Material(glm::vec3 ambC, glm::vec3 diffC, glm::vec3 specC, GLint specExp);
	virtual ~Material();

	void loadIntoProgram(const MaterialUniformLocations &uniLoc, Program &prog) const;

	unsigned int id() const;

	void ambientCoefficient(const glm::vec3 &coefficient);
	void diffuseCoefficient(const glm::vec3 &coefficient);
private:
	glm::vec3 m_AmbientCoefficient;
	glm::vec3 m_DiffuseCoefficient;
	glm::vec3 m_SpecularCoefficient;
	GLint m_SpecularExponent;

	unsigned int m_Id;
	static unsigned int m_NextId;
};

