#version 400

in vec3 ex_color;
out vec4 out_color;

void main(void)
{
	out_color = vec4(ex_color,1.0);
}