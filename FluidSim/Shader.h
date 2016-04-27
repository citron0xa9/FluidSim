#pragma once

#include <GL/glew.h>
#include <string>

class Shader
{
public:
	Shader(GLenum type);
	virtual ~Shader();

	void loadSourceFromString(const std::string& src);
	void loadSourceFromFile(const std::string& path);
	void compile();

	GLuint id() const;
private:
	GLuint m_Id;
	GLenum m_Type;

};

