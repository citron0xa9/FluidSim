#include "Program.h"
#include "Log.h"

GLuint Program::m_usedProgramId = 0;
const UniformNames Program::m_UNIFORM_NAMES{ "matAmbientCoeff", "matDiffuseCoeff", "matSpecularCoeff", "matSpecularExp" };

Program::Program()
{
	m_Id = glCreateProgram();
}


Program::~Program()
{
	if (m_Id == m_usedProgramId) {
		//if this program is used take care of "unusing"
		unuse();
	}
	//detach shaders
	detachAllShaders();

	//finally delete
	glDeleteProgram(m_Id);
}

void Program::attachShader(Shader* shader)
{
	glAttachShader(m_Id, shader->getId());
	m_AttachedShaders.insert(std::make_pair(shader->getId(), shader));
}

void Program::detachAllShaders()
{
	for (auto& elem : m_AttachedShaders) {
		glDetachShader(m_Id, elem.first);
	}
	m_AttachedShaders.clear();
}

void Program::link()
{
	glLinkProgram(m_Id);

	GLint isLinked = 0;
	glGetProgramiv(m_Id, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_Id, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(m_Id, maxLength, &maxLength, errorLog.data());

		//DEBUG("Linking program failed.\n" << errorLog.data());

		//throw exception with msg
		throw std::logic_error(errorLog.data());
	}
	else {
		GLint maxLength = 0;
		glGetProgramiv(m_Id, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		if (maxLength > 1) {
			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(m_Id, maxLength, &maxLength, errorLog.data());
			DEBUG("Linking program succeeded. Log contains:\n" << errorLog.data());
		}
		else {
			DEBUG("Linking program succeeded. Log is empty");
		}
	}
}

void Program::use()
{
	glUseProgram(m_Id);
	m_usedProgramId = m_Id;
}

void Program::unuse()
{
	glUseProgram(0);
	m_usedProgramId = 0;
}

void Program::loadMaterial(const Material& material)
{
	MaterialUniformLocations uniLoc;

	uniLoc.ambientCoefficient = glGetUniformLocation(m_Id, m_UNIFORM_NAMES.ambientCoefficient.c_str());
	uniLoc.diffuseCoefficient = glGetUniformLocation(m_Id, m_UNIFORM_NAMES.diffuseCoefficient.c_str());
	uniLoc.specularCoefficient = glGetUniformLocation(m_Id, m_UNIFORM_NAMES.specularCoefficient.c_str());
	uniLoc.specularExponent = glGetUniformLocation(m_Id, m_UNIFORM_NAMES.specularExponent.c_str());

	material.loadIntoProgram(uniLoc, *this);
}
