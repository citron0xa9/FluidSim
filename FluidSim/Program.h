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
	std::string cameraLookDirection;
	UniformNames(const char *ambC, const char *diffC, const char *specC, const char *specExp, const char *mvpTransform, const char *modTransform, const char *camLookDir) : ambientCoefficient( ambC ), diffuseCoefficient( diffC ), specularCoefficient( specC ),
		specularExponent( specExp ), modelViewProjectTransform( mvpTransform ), modelTransform( modTransform ), cameraLookDirection( camLookDir ) {}
};

class Program
{
public:
	Program(const std::vector<ShaderLightSourceVariable> &lightSrcVars);
	virtual ~Program();

	void attachShader(Shader* shader);
	void detachAllShaders();

	void link();

	void use();
	static void unuse();

	void loadMaterial(const Material &material);
	void loadLights(const std::vector<LightSource*> &lights);

	ShaderLightSourceVariable popFreeLightSourceVariable(const LightSourceType &lightSrcType);
	GLint getUniformLocation(const GLchar *name);
	GLuint getVertexPosIndex() const;
	GLuint getNormalIndex() const;

	GLuint getId() const;

	int getLoadedMaterialId() const;

	void loadModelViewProjectTransform(const glm::mat4x4 &transform);
	void loadModelTransform(const glm::mat4x4 &transform);
	void loadCameraLookDirection(const glm::vec3 &lookDirection);

private:
	GLuint m_Id;
	static GLuint m_usedProgramId;
	std::unordered_map<GLuint,Shader*> m_AttachedShaders;
	std::vector<ShaderLightSourceVariable> m_FreeShaderLightSourceVariables;

	int m_loadedMaterialId;

	static const UniformNames m_UNIFORM_NAMES;
};

