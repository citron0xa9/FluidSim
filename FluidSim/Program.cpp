#include "Program.h"
#include "Log.h"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

GLuint Program::m_usedProgramId = 0;
const UniformNames Program::m_UNIFORM_NAMES{ "matAmbientCoeff", "matDiffuseCoeff", "matSpecularCoeff", "matSpecularExp", "modelViewProjectTransform", "modelTransform", "cameraPosition" };

Program::Program() : m_loadedMaterialId(-1)
{
	m_Id = glCreateProgram();
}

Program::Program(const std::vector<ShaderLightSourceVariable> &lightSrcVars) : m_FreeShaderLightSourceVariables( lightSrcVars ), m_loadedMaterialId(-1)
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
	glAttachShader(m_Id, shader->id());
	m_AttachedShaders.insert(std::make_pair(shader->id(), shader));
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

void Program::use() const
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

	m_loadedMaterialId = material.id();
}

int Program::loadedMaterialId() const
{
	return m_loadedMaterialId;
}

ShaderLightSourceVariable Program::popFreeLightSourceVariable(const LightSourceType &lightSrcType)
{
	auto freeVariable = std::find_if(m_FreeShaderLightSourceVariables.begin(), m_FreeShaderLightSourceVariables.end(),
									[&lightSrcType](const ShaderLightSourceVariable &var){return (var.type == lightSrcType); });

	if (freeVariable == m_FreeShaderLightSourceVariables.end()) {
		throw std::runtime_error("getFreeLightSourceUniformLocation: no free variable found");
	}

	ShaderLightSourceVariable returnValue = *freeVariable;
	m_FreeShaderLightSourceVariables.erase(freeVariable);
	return returnValue;
}

GLint Program::uniformLocation(const GLchar *name) const
{
	return glGetUniformLocation(m_Id, name);
}

GLuint Program::vertexPositionIndex() const
{
	return 0;
}

GLuint Program::normalIndex() const
{
	return 1;
}

void Program::loadLights(const std::list<LightSource*> &lights)
{
	std::for_each(lights.begin(), lights.end(), [this](const LightSource *light){light->loadIntoProgram(*this); });
}

GLuint Program::id() const
{
	return m_Id;
}

void Program::loadModelViewProjectTransform(const glm::mat4x4 &transform)
{
	use();
	GLint location = glGetUniformLocation(m_Id, m_UNIFORM_NAMES.modelViewProjectTransform.c_str());
	if (location == -1) {
		throw std::runtime_error("Error getting uniform location for the mvpTransform");
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
}

void Program::loadModelTransform(const glm::mat4x4 &transform)
{
	use();
	GLint location = glGetUniformLocation(m_Id, m_UNIFORM_NAMES.modelTransform.c_str());
	if (location == -1) {
		throw std::runtime_error("Error getting uniform location for the modelTransform");
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
}

void Program::loadCameraPosition(const glm::vec3 &position)
{
	use();
	GLint location = glGetUniformLocation(m_Id, m_UNIFORM_NAMES.cameraPosition.c_str());
	if (location == -1) {
		throw std::runtime_error("Error getting uniform location for cameraPosition");
	}
	glUniform3fv(location, 1, glm::value_ptr(position));
}