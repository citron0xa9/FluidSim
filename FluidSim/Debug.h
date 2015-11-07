#pragma once

#define GL_DEBUG_WRAP(call)\
{\
	errorCheckVal = glGetError();\
	call;\
	errorCheckVal = glGetError();\
	if (errorCheckVal != GL_NO_ERROR) {\
		throw std::runtime_error("Opengl error: " << gluErrorString(errorCheckVal));\
	}\
}