//#version 330
//
//out vec4 FragColor;
//
//struct Material {
//    sampler2D diffuse;
//    sampler2D specular;
//    float shininess;
//};
//
//struct PointLight {
//    vec3 position;
//
//    float constant;
//    float linear;
//    float quadratic;
//
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//};
//
//
//in vec3 FragPos;
//in vec3 Normal;
//in vec2 TexCoords;
//
//uniform vec3 viewPos;
//uniform PointLight pointLight;
//uniform Material material;
//
//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//
//void main()
//{
//    // properties
//    vec3 norm = normalize(Normal);
//    vec3 viewDir = normalize(viewPos - FragPos);
//    
//    vec3 result = CalcPointLight(pointLight, norm, FragPos, viewDir);
//
//    FragColor = vec4(result, 1.0f);
//}
//
//
//// calculates the color when using a point light.
//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//    vec3 lightDir = normalize(light.position - fragPos);
//    // diffuse shading
//    float diff = max(dot(normal, lightDir), 0.0);
//    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    // attenuation
//    float distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
//    // combine results
//    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
//    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
//    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
//    ambient *= attenuation;
//    diffuse *= attenuation;
//    specular *= attenuation;
//    return (ambient + diffuse + specular);
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#version 330

#define LIGHTS_NUM 2

struct FLight {
	vec4 lightPos;
	vec4 lightColour;
	float intensity;
	float constant;
	float linear;
	float quadratic;
};

uniform mat4 V;
uniform mat4 M;

uniform sampler2D textureMap;
uniform sampler2D textureMapSpec;
uniform sampler2D textureMapEnv;
uniform sampler2D textureMapBump;
uniform float phongExponent;
uniform float shininess;
//Single light/////////////////////////////////////
//uniform vec4 lightColour;
uniform float gamma;
//uniform float constant;
//uniform float linear;
//uniform float quadratic;
//uniform float intensity;
uniform FLight f_light[LIGHTS_NUM];
///////////////////////////////////////

//Multiple lights/////////////////////////////////////

//struct Light {
//	vec4 lightPos;
//
//	float constant;
//	float linear;
//	float quadratic;
//};

///////////////////////////////////////
//uniform vec3 lightPos;
//uniform vec3 lightColor;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

//Zmienne interpolowane
//in vec3 Normal;
//in vec3 FragPos;
in vec3 vertex;
in vec2 iTex;
in vec2 iTex1;
in vec3 n;
//in vec4 l;
in vec3 v;
//in int hasBump;
//in float distance;
//in Light{
//	vec4 lightPos;
//	float intensity;
//}inLight;

//in mat4 Vfrag;
//in mat4 Mfrag;
//in vec3 vertexfrag;

vec4 CalculateLight(FLight light, vec4 norm_n, vec4 norm_v)
{
	vec4 norm_l = normalize(light.lightPos - norm_v);
	//vec4 norm_l = normalize(V * light.lightPos - V * M * vec4(vertex, 1.0f));
	float distance = length(light.lightPos - norm_v);

	vec4 r = reflect(-norm_l, norm_n);

	//vec4 kdiffuse = mix(texture(textureMap, iTex), texture(textureMapEnv, iTex1), 0.1f);
	//vec4 kspecular = mix(texture(textureMapSpec, iTex), texture(textureMapEnv, iTex1), .1f);
	vec4 kdiffuse = texture(textureMap, iTex);
	if (kdiffuse.a < 0.4)
		discard;
	vec4 kspecular = texture(textureMapSpec, iTex);
	////vec4 kbump = texture(textureMapBump, iTex);

	//float nl = clamp(dot(norm_n, norm_l), 0, 1);
	float nl = max(dot(norm_n, norm_l), 0.0);
	float rv = pow(clamp(dot(r, norm_v), 0, 1), phongExponent);

	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	//vec4 colorObj = texture(tex, iTex);
	//float gamma = 0.7f;
	return (vec4(kdiffuse.rgb * nl, kdiffuse.a) * attenuation * light.lightColour + vec4(kspecular.rgb * rv * shininess, 0) * attenuation * light.lightColour) * light.intensity;
	//pixelColor.rgb = pow(pixelColor.rgb, vec3(1.0 / gamma));
}

void main(void) {
	vec4 norm_n = vec4(normalize(n), 1.0);
	vec4 norm_v = vec4(normalize(v), 1.0);
	vec4 result = vec4(0, 0, 0, 1);
	for (int i = 0; i < LIGHTS_NUM; i++)
	{
		result += CalculateLight(f_light[i], norm_n, norm_v);
	}
	pixelColor = result;
	// OLD
	/*float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec4 colorObj = texture(tex, i_texCord);

	vec3 result = (ambient + diffuse) * colorObj.rgb;
	pixelColor = vec4(result, 1.0);*/

	// NEW
	/*if (hasBump == 0)
	{*/

	////single light
	//vec4 norm_l = normalize(l);
	//vec4 norm_n = normalize(n);
	//vec4 norm_v = normalize(v);

	//vec4 r = reflect(-norm_l, norm_n);

	////vec4 kdiffuse = mix(texture(textureMap, iTex), texture(textureMapEnv, iTex1), 0.1f);
	////vec4 kspecular = mix(texture(textureMapSpec, iTex), texture(textureMapEnv, iTex1), .1f);
	//vec4 kdiffuse = texture(textureMap, iTex);
	//if (kdiffuse.a < 0.4)
	//	discard;
	//vec4 kspecular = texture(textureMapSpec, iTex);
	//////vec4 kbump = texture(textureMapBump, iTex);

	////float nl = clamp(dot(norm_n, norm_l), 0, 1);
	//float nl = max(dot(norm_n, norm_l), 0.0);
	//float rv = pow(clamp(dot(r, norm_v), 0, 1), phongExponent);

	//float attenuation = 1.0 / (f_light.constant + f_light.linear * distance + f_light.quadratic * (distance * distance));
	//
	////vec4 colorObj = texture(tex, iTex);
	////float gamma = 0.7f;
	//pixelColor = (vec4(kdiffuse.rgb * nl, kdiffuse.a) * attenuation * f_light.lightColour + vec4(kspecular.rgb * rv * shininess, 0) * attenuation * f_light.lightColour) * f_light.intensity;
	//pixelColor.rgb = pow(pixelColor.rgb, vec3(1.0 / gamma));
	//pixelColor = kbump;
/*}*/

////////////////////

//NEW + normalmap
//if (hasBump == 1)
//{
	//vec4 mv = normalize(v);
	//vec4 ml = normalize(l);
	//vec4 mn = normalize(vec4(texture(textureMapBump, iTex).rgb * 2 - 1, 0));
	////vec4 mn = vec4(0, 0, 1, 0);
	//vec4 mr = reflect(-ml, mn);

	//vec4 kd = texture(textureMap, iTex);
	//if (kd.a < 0.1)
	//	discard;
	//vec4 ks = texture(textureMapSpec, iTex);

	//float nl = clamp(dot(mn, ml), 0, 1);
	//float rv = pow(clamp(dot(mr, mv), 0, 1), 2);
	//pixelColor = vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb * rv * 2.0f, 0);
//}

//multi light //////////////////////////////
	//vec4 norm_n = normalize(n);
	//vec4 norm_v = normalize(v);

	//vec4 result;

}