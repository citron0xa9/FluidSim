#pragma once

#include <GL/glew.h>
#include <string>

class Shader
{
public:
	Shader(GLenum type);
	virtual ~Shader();

	void setSource(const std::string& src);
	void setSourcePath(const std::string& path);
	void compile();

	GLuint getId() const;
private:
	GLuint m_Id;
	GLenum m_Type;

};

