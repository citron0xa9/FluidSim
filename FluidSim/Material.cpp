#include "Material.h"


Material::Material(glm::vec3 ambC, glm::vec3 diffC, glm::vec3 specC, GLint specExp) : m_AmbientCoefficient{ ambC }, m_DiffuseCoefficient{ diffC },
m_SpecularCoefficient{ specC }, m_SpecularExponent{specExp}
{

}


Material::~Material()
{
}

void Material::loadIntoProgram(const MaterialUniformLocations &uniLoc, Program &prog)
{
	prog.use();
	
	glUniform3fv(uniLoc.ambientCoefficient, 1, &m_AmbientCoefficient[0]);
	glUniform3fv(uniLoc.diffuseCoefficient, 1, &m_DiffuseCoefficient[0]);
	glUniform3fv(uniLoc.specularCoefficient, 1, &m_SpecularCoefficient[0]);
	glUniform1i(uniLoc.specularExponent, m_SpecularExponent);
}
