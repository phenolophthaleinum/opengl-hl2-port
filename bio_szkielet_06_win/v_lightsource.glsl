#version 330

//Vertex shader

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

in vec4 vertex;

void main()
{
	gl_Position = P * V * M * vertex;
}