#version 450

#define PI 3.1415

in vec3 ex_position;
in vec3 ex_normal;

out vec4 out_color;

uniform vec3 matAmbientCoeff;
uniform vec3 matDiffuseCoeff;
uniform vec3 matSpecularCoeff;
uniform int matSpecularExp;

uniform float sunLight_Intensity;
uniform vec3 sunLight_Direction;

uniform vec3 cameraPosition;

float ambientLight_Intensity = 0.2;

void main(void)
{
	vec3 inv_normal_normalized = normalize(ex_normal) * -1;
	vec3 lightDirNorm = normalize(sunLight_Direction);
	vec3 lookDirNorm = normalize(ex_position-cameraPosition);

	vec3 I_amb = ambientLight_Intensity * matAmbientCoeff;
	vec3 I_diff = sunLight_Intensity * matDiffuseCoeff * max(0.0, dot(inv_normal_normalized, lightDirNorm));

	float specNormFactor = (matSpecularExp+2)/(2*PI);
	vec3 idealReflectNorm = normalize(reflect(lightDirNorm, inv_normal_normalized));
	vec3 I_spec = sunLight_Intensity * specNormFactor * pow(max(0.0, -dot(lookDirNorm, idealReflectNorm)), matSpecularExp) * matSpecularCoeff;
	vec3 final_color = clamp(I_amb + I_diff + I_spec, 0.0, 1.0);
	out_color = vec4(final_color, 1.0);
}
