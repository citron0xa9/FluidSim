#version 400

layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_normal;

out vec3 ex_position;
out vec3 ex_normal;

uniform mat4 modelViewProjectTransform;
uniform mat4 modelTransform;

void main(void)
{
	gl_Position = modelViewProjectTransform * vec4(in_position,1.0);
	ex_position = (modelTransform * vec4(in_position, 1.0)).xyz;
	ex_normal = (modelTransform * vec4(in_normal, 1.0)).xyz;
}
