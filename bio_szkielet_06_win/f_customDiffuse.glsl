#version 330

#define LIGHTS_NUM 10

struct FLight {
	vec4 lightColour;
	float intensity;
	float constant;
	float linear;
	float quadratic;
};

uniform int lights_count;
uniform sampler2D textureMap;
uniform sampler2D textureMapSpec;
uniform sampler2D textureMapEnv;
uniform sampler2D textureMapBump;
uniform float phongExponent;
uniform float shininess;
uniform float gamma;
uniform FLight f_light[LIGHTS_NUM];

out vec4 pixelColor;

in vec2 iTex;
in vec2 iTex1;
in vec4 n;
in vec4 l[LIGHTS_NUM];
in vec4 v;
in float distance[LIGHTS_NUM];

vec4 CalculateLight(FLight light, vec4 norm_l, vec4 norm_n, vec4 norm_v, float dist)
{

	vec4 r = reflect(-norm_l, norm_n);

	vec4 kdiffuse = texture(textureMap, iTex);
	if (kdiffuse.a < 0.4)
		discard;
	vec4 kspecular = texture(textureMapSpec, iTex);

	float nl = max(dot(norm_n, norm_l), 0.0);
	float rv = pow(clamp(dot(r, norm_v), 0, 1), phongExponent);

	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	return (vec4(kdiffuse.rgb * nl, kdiffuse.a) * attenuation * light.lightColour + vec4(kspecular.rgb * rv * shininess, 0) * attenuation * light.lightColour) * light.intensity;
	//pixelColor.rgb = pow(pixelColor.rgb, vec3(1.0 / gamma));
}

void main(void) 
{
	vec4 norm_n = normalize(n);
	vec4 norm_v = normalize(v);
	vec4 result = vec4(0, 0, 0, 1);

	for (int i = 0; i < lights_count; i++)
	{
		vec4 norm_l = normalize(l[i]);
		float dist = distance[i];
		result += CalculateLight(f_light[i], norm_l, norm_n, norm_v, dist);
	}

	pixelColor = result;
	pixelColor.rgb = pow(pixelColor.rgb, vec3(1.0 / gamma));
}
