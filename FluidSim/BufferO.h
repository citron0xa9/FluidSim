#pragma once

#include <vector>
#include <GL/glew.h>

class BufferO
{
public:

	void bind() const;
	void unbind() const;
	static void unbindStatic(GLenum bufferType);

	template <typename T> void pushData(const std::vector<T>& v, GLenum usage, bool bindBefore)
	{
		if (bindBefore) {
			bind();
		}
		glBufferData(m_bufferType, v.size()*sizeof(T), v.data(), usage);
	}

	GLuint getId() const;
protected:
	BufferO(GLenum bufferType);	//protected because of m_boundId of childs / unbind has to be performed, if neccessary, by childs
	virtual ~BufferO();
private:
	GLuint m_Id;
	GLenum m_bufferType;
};

