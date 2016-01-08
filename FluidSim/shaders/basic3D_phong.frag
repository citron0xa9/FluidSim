#version 400

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

uniform vec3 cameraLookAt;

float ambientLight_Intensity = 1.0;

void main(void)
{
	vec3 normal_normalized = normalize(ex_normal);
	vec3 lightDirNorm = normalize(sunLight_Direction);
	vec3 lookAtNorm = normalize(cameraLookAt);

	vec3 I_amb = ambientLight_Intensity * matAmbientCoeff;
	vec3 I_diff = sunLight_Intensity * matDiffuseCoeff * max(0.0, dot(normal_normalized, lightDirNorm));

	float specNormFactor = (matSpecularExp+2)/(2*PI);
	vec3 idealReflectNorm = normalize(reflect(lightDirNorm, normal_normalized));
	vec3 I_spec = sunLight_Intensity * specNormFactor * pow(max(0.0, -dot(lookAtNorm, idealReflectNorm)), matSpecularExp) * matSpecularCoeff;
	vec3 final_color = clamp(I_amb + I_diff + I_spec, 0.0, 1.0);
	out_color = vec4(1.0, 0.0, 0.0, 1.0);
}
