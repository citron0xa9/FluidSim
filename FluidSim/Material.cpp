#include "Material.h"
#include "Program.h"


unsigned int Material::m_NextId = 0;

Material::Material(glm::vec3 ambC, glm::vec3 diffC, glm::vec3 specC, GLint specExp) : m_AmbientCoefficient{ ambC }, m_DiffuseCoefficient{ diffC },
m_SpecularCoefficient{ specC }, m_SpecularExponent{ specExp }, m_Id{m_NextId}
{
	m_NextId++;
}


Material::~Material()
{
}

void Material::loadIntoProgram(const MaterialUniformLocations &uniLoc, Program &prog) const
{
	prog.use();
	
	glUniform3fv(uniLoc.ambientCoefficient, 1, &m_AmbientCoefficient[0]);
	glUniform3fv(uniLoc.diffuseCoefficient, 1, &m_DiffuseCoefficient[0]);
	glUniform3fv(uniLoc.specularCoefficient, 1, &m_SpecularCoefficient[0]);
	glUniform1i(uniLoc.specularExponent, m_SpecularExponent);
}

unsigned int Material::id() const
{
	return m_Id;
}