#pragma once

#include <GL/glew.h>
#include <algorithm>

struct IndexCombination {
	union {
		struct {
			GLushort vertexIndex;
			GLushort normalIndex;
		};
		GLushort indices[2];
	};
	GLushort vboIndex;	//at which index lays this combination in the vbo
};

bool operator==(const IndexCombination& a, const IndexCombination& b);