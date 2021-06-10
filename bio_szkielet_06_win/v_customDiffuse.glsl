#version 330

#define LIGHTS_NUM 10

struct VLight {
    vec4 lightPos;
};

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform int lights_count;
uniform VLight v_light[LIGHTS_NUM];

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord; 
in vec3 tangent;
in vec3 binormal;


out vec2 iTex;
out vec2 iTex1;
out vec4 n;
out vec4 l[LIGHTS_NUM];
out vec4 v;
out float distance[LIGHTS_NUM];

float CalculateDistance(VLight light)
{
    return length(light.lightPos - M * vec4(vertex, 1.0f));
}

vec4 CalculateL(VLight light)
{
    return normalize(V * light.lightPos - V * M * vec4(vertex, 1.0f));
}

void main(void)
{
    n = normalize(V * M * vec4(normal, 0.0f));
    v = normalize(vec4(0, 0, 0, 1) - V * M * vec4(vertex, 1.0f));
    iTex = texCoord;

    for (int i = 0; i < lights_count; i++)
    {
        l[i] = CalculateL(v_light[i]);
        distance[i] = CalculateDistance(v_light[i]);
    }

    gl_Position = P * V * M * vec4(vertex, 1.0f);
}
