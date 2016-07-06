#version 450

layout(location=0) in vec3 in_position;

uniform mat4 modelViewProjectTransform;

void main(void)
{
	gl_Position = modelViewProjectTransform * vec4(in_position,1.0);
}
