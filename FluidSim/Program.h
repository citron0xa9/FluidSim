#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <unordered_map>
#include <string>
#include "Shader.h"
#include "Material.h"
#include "LightSource.h"

struct UniformNames {
	std::string ambientCoefficient;
	std::string diffuseCoefficient;
	std::string specularCoefficient;
	std::string specularExponent;
	std::string modelViewProjectTransform;
	std::string modelTransform;
	std::string cameraPosition;
	UniformNames(const char *ambC, const char *diffC, const char *specC, const char *specExp, const char *mvpTransform, const char *modTransform, const char *camPos) : ambientCoefficient( ambC ), diffuseCoefficient( diffC ), specularCoefficient( specC ),
		specularExponent( specExp ), modelViewProjectTransform( mvpTransform ), modelTransform( modTransform ), cameraPosition(camPos) {}
};

class Program
{
public:
	Program();
	Program(const std::vector<ShaderLightSourceVariable> &lightSrcVars);
	virtual ~Program();

	void attachShader(Shader* shader);
	void detachAllShaders();

	void link();

	void use() const;
	static void unuse();

	void loadMaterial(const Material &material);
	void loadLights(const std::list<LightSource*> &lights);

	ShaderLightSourceVariable popFreeLightSourceVariable(const LightSourceType &lightSrcType);
	GLint uniformLocation(const GLchar *name) const;
	GLuint vertexPositionIndex() const;
	GLuint normalIndex() const;

	GLuint id() const;

	int loadedMaterialId() const;

	void loadModelViewProjectTransform(const glm::mat4x4 &transform);
	void loadModelTransform(const glm::mat4x4 &transform);
	void loadCameraPosition(const glm::vec3 &position);

private:
	void resetLights();

	GLuint m_Id;
	static GLuint m_usedProgramId;
	std::unordered_map<GLuint,Shader*> m_AttachedShaders;
	std::vector<ShaderLightSourceVariable> m_InitialFreeShaderLightSourceVariables;
	std::vector<ShaderLightSourceVariable> m_FreeShaderLightSourceVariables;

	int m_loadedMaterialId;

	static const UniformNames m_UNIFORM_NAMES;
};

