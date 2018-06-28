#version 400

layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_color;

out vec3 ex_color;

void main(void)
{
	gl_Position = vec4(in_position,1.0);
	ex_color = in_color;
}