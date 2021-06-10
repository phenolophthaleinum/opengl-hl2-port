#version 330

//max lights
#define LIGHTS_NUM 10

//light
struct FLight {
	vec4 lightColour;
	float intensity;
	float constant;
	float linear;
	float quadratic;
};

//textures
uniform sampler2D textureMap;
uniform sampler2D textureMapSpec;
uniform sampler2D textureMapEnv;
uniform sampler2D textureMapBump;

//current lights number on scene
uniform int lights_count;

//misc properties
uniform float phongExponent;
uniform float shininess;
uniform float gamma;

//lights list
uniform FLight f_light[LIGHTS_NUM];


out vec4 pixelColor;


in vec2 iTex;
in vec2 iTex1;
in vec4 n;
in vec4 l[LIGHTS_NUM];
in vec4 v;
in float distance[LIGHTS_NUM];

vec4 CalculateLight(FLight light, vec4 mv, vec4 ml, vec4 mn, float dist)
{
	vec4 mr = reflect(-ml, mn);

	vec4 kd = texture(textureMap, iTex);
	if (kd.a < 0.4)
		discard;
	vec4 ks = texture(textureMapSpec, iTex);

	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	float nl = max(dot(mn, ml), 0.0);
	float rv = pow(clamp(dot(mr, mv), 0, 1), phongExponent);
	return (vec4(kd.rgb * nl, kd.a) * attenuation * light.lightColour + vec4(ks.rgb * rv * shininess, 0) * attenuation * light.lightColour) * light.intensity;
	//pixelColor.rgb = pow(pixelColor.rgb, vec3(1.0 / gamma));
}

void main(void) 
{
	vec4 mv = normalize(v);
	vec4 mn = normalize(vec4(texture(textureMapBump, iTex).rgb * 2 - 1, 0));
	vec4 result = vec4(0, 0, 0, 1);

	for (int i = 0; i < lights_count; i++)
	{
		vec4 ml = normalize(l[i]);
		float dist = distance[i];
		result += CalculateLight(f_light[i], mv, ml, mn, dist);
	}
	pixelColor = result;
	pixelColor.rgb = pow(pixelColor.rgb, vec3(1.0 / gamma));
}