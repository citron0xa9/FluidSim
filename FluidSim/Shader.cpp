#include "Shader.h"
#include "Log.h"
#include <fstream>
#include <vector>


Shader::Shader(GLenum type)
{
	m_Id = glCreateShader(type);
}


Shader::~Shader()
{
	glDeleteShader(m_Id);
}

void Shader::loadSourceFromString(const std::string& src)
{
	const GLchar *cStr = src.c_str();
	glShaderSource(m_Id, 1, &cStr, NULL);
}

void Shader::loadSourceFromFile(const std::string& path)
{
	std::ifstream srcFile;
	std::string srcStr;

	srcFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); //calling func has to deal with exceptions
	srcFile.open(path);
	std::string line;
	while (!srcFile.eof()) {
		try {
			std::getline(srcFile, line);
		}
		catch (const std::exception &ex) {
			if (!srcFile.eof()) {
				throw ex;
			}
			else {
				continue;
			}
		}
		srcStr = srcStr + line + "\n";
	}
	srcFile.close();

	loadSourceFromString(srcStr);
}

void Shader::compile() {
	glCompileShader(m_Id);

	GLint isCompiled = 0;
	glGetShaderiv(m_Id, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(m_Id, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(m_Id, maxLength, &maxLength, errorLog.data());

		//DEBUG("Compiling shader failed.\n" << errorLog.data());

		//throw exception with msg
		throw std::logic_error(errorLog.data());
	}
	else {
		GLint maxLength = 0;
		glGetShaderiv(m_Id, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength > 1) {
			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(m_Id, maxLength, &maxLength, errorLog.data());

			DEBUG("Successfully compiled shader, log contains:\n" << errorLog.data());
		}
		else {
			DEBUG("Successfully compiled shader, log is empty");
		}
	}
}

GLuint Shader::id() const
{
	return m_Id;
}
