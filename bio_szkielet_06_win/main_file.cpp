/*
HALF LIFE 2: Bootleg

An OpenGL experiment we were forced to do.
This program runs converted Source Engine map made in Hammer Editor in OpenGL.

The best thing about this is that it exists. And supports multiple lights (up to 10) and normal mapping. 
Contains integrated ImGui.
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include <iostream>
#include "OBJ_Loader.h"
#include <filesystem>
#include <windows.h>
#include <mmsystem.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// *** Important ImGui extension - allows working with vectors ***
namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

}

// *** Light definition ***
class Light {
public:
	std::string Name;
	glm::vec3 Position;
	glm::vec4 Colour;
	float Constant;
	float Linear;
	float Quadratic;
	float Intensity;

	Light(std::string n, glm::vec3 pos, glm::vec4 color, float c, float l, float q, float i)
	{
		Name = n;
		Position = pos;
		Colour = color;
		Constant = c;
		Linear = l;
		Quadratic = q;
		Intensity = i;
	}
};

using namespace std;

int consolePress = 1;

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1.333f;

glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = 800 / 2, lastY = 600 / 2;
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float fov = 60.0f;

float cameraSpeed = 0.01f;
float cameraSpeedMultiply = 7.0f;
float lightSpeed = 0.01f;

float renderDistance = 3000.0f;

const char* glsl_version = "#version 130";

glm::vec4 lightPos = glm::vec4(-0.2f, -1.0f, -0.3f, 1.0f);
glm::vec3 lightPosR[] = { 
	glm::vec3(0.0f, 2.0f, 0.0f),
	glm::vec3(5.0f, 1.0f, 0.0f),
};
glm::vec4 lightColour[] = {
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
};
float phongExponent = 2.0f;
float shininess = 1.0f;
float gamma = 1.0f;
float constant[] = {
	1.0f,
	1.0f
};
float linear[] = {
	0.09f,
	0.09f
};
float quadratic[] = {
	0.032f,
	0.032f
};
float intensity[] = {
	1.0f,
	1.0f
};

std::vector<Light> Lights = { 
	Light("Light0", glm::vec3(500.0f, 1000.0f, 500.0f), glm::vec4(1.0f, 0.952f, 0.784f, 1.0f), 1.0f, 0.019f, 0.0f, 31.161f)
	//Light("Light1", glm::vec3(5.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, 1.0f) 
};

std::vector<std::string> LightsNames = {
	"Light0"
};

std::vector<std::string> usedTextures;
std::vector<GLuint> globalTextures;

objl::Loader loader;
GLuint cube_texture;
GLuint cube_textureSpec;
GLuint texture_env;
GLuint skybox_texture;

vector<const char*> faces
{
	/*"skybox/sky_lostcoastrt.png",
	"skybox/sky_lostcoastlf.png",
	"skybox/sky_lostcoastup.png",
	"skybox/sky_lostcoastdn.png",
	"skybox/sky_lostcoastft.png",
	"skybox/sky_lostcoastbk.png"*/

	// *** This is the correct order of skybox textures imported from Source Engine ***
	"skybox/sky_lostcoastbk.png",
	"skybox/sky_lostcoastft.png",
	"skybox/sky_lostcoastup180.png",
	"skybox/sky_lostcoastdn.png",
	"skybox/sky_lostcoastlf.png",
	"skybox/sky_lostcoastrt.png"
};

unsigned int cubemapTexture;

//GLuint readTexture(const char* filename) {
//	GLuint texture;
//	//glActiveTexture(GL_TEXTURE0);
//
//	//Wczytanie do pamięci komputera
//	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
//	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
//	//Wczytaj obrazek
//	unsigned error = lodepng::decode(image, width, height, filename);
//
//	//Import do pamięci karty graficznej
//	glGenTextures(1, &texture); //Zainicjuj jeden uchwyt
//	glBindTexture(GL_TEXTURE_2D, texture); //Uaktywnij uchwyt
//	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
//	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
//		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	return texture;
//}

class LoadedMesh {
public:
	std::vector<float> Verticies;
	std::vector<unsigned int> Indices;
	std::vector<float> TexCoords;
	std::vector<float> Normals;
	std::vector<float> Tangents;
	std::vector <float> Binormals;
	std::string DiffuseMaterial;
	std::string SpecMaterial;
	std::string BumpMaterial;
	GLuint Texture;
	GLuint Texture1;
	GLuint Texture2;
	int hasBumpMap;

	LoadedMesh(std::vector<float> v, std::vector<float> t, std::vector<unsigned int> i, std::vector<float> n, std::string m,
		std::string m1, std::string m2) {
		Verticies = v;
		TexCoords = t;
		Indices = i;
		DiffuseMaterial = m;
		SpecMaterial = m1;
		BumpMaterial = m2;
		Normals = n;
		hasBumpMap = 0;
		if (BumpMaterial.find(".png") != std::string::npos)
		{
			hasBumpMap = 1;
			cout << "Found bumpmap" << endl;

		//// GIGA TANGENT SPACE CODE ////
			std::vector<glm::vec3> tangent;
			std::vector<glm::vec3> binormal;

			std::vector<float>* tangents = new std::vector<float>(Verticies.size(), 0);
			std::vector<float>* binormals = new std::vector<float>(Verticies.size(), 0);

			cout << Indices.size() << endl;

			for (int i = 0; i < Indices.size(); i = i + 3)
			{
				glm::vec3 vertex0 = glm::vec3(Verticies.at(3 * Indices.at(i)), Verticies.at(3 * Indices.at(i) + 1), Verticies.at(3 * Indices.at(i) + 2));
				glm::vec3 vertex1 = glm::vec3(Verticies.at(3 * Indices.at(i + 1)), Verticies.at(3 * Indices.at(i + 1) + 1), Verticies.at(3 * Indices.at(i + 1) + 2));
				glm::vec3 vertex2 = glm::vec3(Verticies.at(3 * Indices.at(i + 2)), Verticies.at(3 * Indices.at(i + 2) + 1), Verticies.at(3 * Indices.at(i + 2) + 2));
			
				cout << vertex0.x << "; " << vertex0.y << "; " << vertex0.z << "; " << endl;
				cout << vertex1.x << "; " << vertex1.y << "; " << vertex1.z << "; " << endl;
				cout << vertex2.x << "; " << vertex2.y << "; " << vertex2.z << "; " << endl;

				glm::vec2 uv0 = glm::vec2(TexCoords.at(2 * Indices.at(i)), TexCoords.at(2 * Indices.at(i) + 1));
				glm::vec2 uv1 = glm::vec2(TexCoords.at(2 * Indices.at(i + 1)), TexCoords.at(2 * Indices.at(i + 1) + 1));
				glm::vec2 uv2 = glm::vec2(TexCoords.at(2 * Indices.at(i + 2)), TexCoords.at(2 * Indices.at(i + 2) + 1));

				//NEW
				glm::vec3 e1 = vertex1 - vertex0;
				glm::vec3 e2 = vertex2 - vertex0;

				glm::vec2 deltaUV1 = uv1 - uv0;
				glm::vec2 deltaUV2 = uv2 - uv0;

				//NEW
				float r = (float)(1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x));

				glm::vec3 tan; // tangents
				glm::vec3 bin; // binormal
				
				//NEW
				tan = glm::normalize((e1 * deltaUV2.y - (e2 * deltaUV1.y)) * r);
				bin = glm::normalize((e2 * deltaUV1.x - (e1 * deltaUV2.x)) * r);

				(*tangents)[3 * Indices.at(i)] = tan.x;
				(*tangents)[3 * Indices.at(i) + 1] = tan.y;
				(*tangents)[3 * Indices.at(i) + 2] = tan.z;

				(*tangents)[3 * Indices.at(i + 1)] = tan.x;
				(*tangents)[3 * Indices.at(i + 1) + 1] = tan.y;
				(*tangents)[3 * Indices.at(i + 1) + 2] = tan.z;

				(*tangents)[3 * Indices.at(i + 2)] = tan.x;
				(*tangents)[3 * Indices.at(i + 2) + 1] = tan.y;
				(*tangents)[3 * Indices.at(i + 2) + 2] = tan.z;

				(*binormals)[3 * Indices.at(i)] = bin.x;
				(*binormals)[3 * Indices.at(i) + 1] = bin.y;
				(*binormals)[3 * Indices.at(i) + 2] = bin.z;

				(*binormals)[3 * Indices.at(i + 1)] = bin.x;
				(*binormals)[3 * Indices.at(i + 1) + 1] = bin.y;
				(*binormals)[3 * Indices.at(i + 1) + 2] = bin.z;

				(*binormals)[3 * Indices.at(i + 2)] = bin.x;
				(*binormals)[3 * Indices.at(i + 2) + 1] = bin.y;
				(*binormals)[3 * Indices.at(i + 2) + 2] = bin.z;

				cout << "Done: " << i << endl;
			}

			for (int i = 0; i < Verticies.size(); i++)
			{
				Tangents.push_back((*tangents)[i]);
				Binormals.push_back((*binormals)[i]);
			}

			cout << Tangents[0] << endl;
		}
		//////////////////////////////////////////


		auto it = std::find(usedTextures.begin(), usedTextures.end(), m);
		if (it != usedTextures.end()) {
			size_t tex_id = it - usedTextures.begin();
			Texture = globalTextures[tex_id];
		}
		else {
			cout << m.data();
			auto tex = this->readTexture(m.data());
			globalTextures.emplace_back(tex);
			usedTextures.emplace_back(m.data());
			Texture = tex;
		}

		auto it2 = std::find(usedTextures.begin(), usedTextures.end(), m1);
		if (it2 != usedTextures.end()) {
			size_t tex_id = it2 - usedTextures.begin();
			Texture1 = globalTextures[tex_id];
		}
		else {
			cout << m1.data();
			auto tex = this->readTexture(m1.data());
			globalTextures.emplace_back(tex);
			usedTextures.emplace_back(m1.data());
			Texture1 = tex;
		}

		auto it3 = std::find(usedTextures.begin(), usedTextures.end(), m2);
		if (it3 != usedTextures.end()) {
			size_t tex_id = it3 - usedTextures.begin();
			Texture2 = globalTextures[tex_id];
		}
		else {
			cout << m2.data();
			auto tex = this->readTexture(m2.data());
			globalTextures.emplace_back(tex);
			usedTextures.emplace_back(m2.data());
			Texture2 = tex;
		}
	}

	GLuint readTexture(const char* filename) {
		GLuint tex;
		glActiveTexture(GL_TEXTURE0);

		std::vector<unsigned char> image; 
		unsigned width, height;
		unsigned error = lodepng::decode(image, width, height, filename);
		if (error)
			std::cerr << "ERROR WHILE LOADING TEXTURE. CODE: " << error << "Image: " << filename << std::endl;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

		glGenerateMipmap(GL_TEXTURE_2D);

		GLfloat value, max_anisotropy = 8.0f; /* don't exceed this value...*/
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);

		value = (value > max_anisotropy) ? max_anisotropy : value;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);

		std::cout << "HERE: " << filename << std::endl;

		return tex;
	}
};

class LoadedModel {
public:
	std::vector<LoadedMesh> Meshes;

	bool loadModel(std::string path, bool inv = true) {
		bool good = loader.LoadFile(path);
		if (!good)
			return false;

		for (auto& mesh : loader.LoadedMeshes) {
			std::vector<float> vertices;
			std::vector<float> texCoords;
			std::vector <float> normals;
			for (auto& vert : mesh.Vertices) {

				vertices.emplace_back(vert.Position.X);
				vertices.emplace_back(vert.Position.Y);
				vertices.emplace_back(vert.Position.Z);

				normals.emplace_back(vert.Normal.X);
				normals.emplace_back(vert.Normal.Y);
				normals.emplace_back(vert.Normal.Z);

				texCoords.emplace_back(vert.TextureCoordinate.X);
				texCoords.emplace_back(inv ? 1.0f - vert.TextureCoordinate.Y : vert.TextureCoordinate.Y);
			}


			std::filesystem::path model_dir = std::filesystem::path(path).parent_path();
			cout << mesh.MeshMaterial.map_Kd;
			cout << mesh.MeshMaterial.map_Ks;
			cout << mesh.MeshMaterial.map_bump;
			std::string texName = (model_dir / mesh.MeshMaterial.map_Kd).u8string();
			std::string tex1Name = (model_dir / mesh.MeshMaterial.map_Ks).u8string();
			std::string tex2Name = (model_dir / mesh.MeshMaterial.map_bump).u8string();
			Meshes.emplace_back(LoadedMesh(vertices, texCoords, mesh.Indices, normals, texName, tex1Name, tex2Name));

		}


		return true;
	}

	std::string extractDir(std::string path) {
		std::string directory;
		const size_t last_slash_idx = path.rfind('\\');
		if (std::string::npos != last_slash_idx)
		{
			directory = path.substr(0, last_slash_idx);
		}
		return directory;
	}

	void drawModel(glm::mat4 M, glm::mat4 P, glm::mat4 V) 
	{
		for (auto& mesh : Meshes)
		{
			if (mesh.hasBumpMap == 1)
			{
				// init and pass matrices
				spBumpmapped->use();
				glUniformMatrix4fv(spBumpmapped->u("P"), 1, false, glm::value_ptr(P));
				glUniformMatrix4fv(spBumpmapped->u("V"), 1, false, glm::value_ptr(V));
				glUniformMatrix4fv(spBumpmapped->u("M"), 1, false, glm::value_ptr(M));

				//pass phong exp
				glUniform1f(spBumpmapped->u("phongExponent"), phongExponent);
				
				// pass shininess
				glUniform1f(spBumpmapped->u("shininess"), shininess);

				// pass gamma component
				glUniform1f(spBumpmapped->u("gamma"), gamma);

				// pass current number of lights on scene
				glUniform1i(spBumpmapped->u("lights_count"), Lights.size());

				// pass light info for all lights on scene
				for (int i = 0; i < Lights.size(); i++)
				{
					glUniform4f(spBumpmapped->u(("v_light[" + std::to_string(i) + "].lightPos").c_str()), Lights.at(i).Position.x, Lights.at(i).Position.y, Lights.at(i).Position.z, 1.0f);
					glUniform4fv(spBumpmapped->u(("f_light[" + std::to_string(i) + "].lightColour").c_str()), 1, glm::value_ptr(Lights.at(i).Colour));
					glUniform1f(spBumpmapped->u(("f_light[" + std::to_string(i) + "].constant").c_str()), Lights.at(i).Constant);
					glUniform1f(spBumpmapped->u(("f_light[" + std::to_string(i) + "].linear").c_str()), Lights.at(i).Linear);
					glUniform1f(spBumpmapped->u(("f_light[" + std::to_string(i) + "].quadratic").c_str()), Lights.at(i).Quadratic);
					glUniform1f(spBumpmapped->u(("f_light[" + std::to_string(i) + "].intensity").c_str()), Lights.at(i).Intensity);
				}

				// pass array data to bumpmapped shader
				glEnableVertexAttribArray(spBumpmapped->a("vertex"));
				glVertexAttribPointer(spBumpmapped->a("vertex"), 3, GL_FLOAT, false, 0, mesh.Verticies.data());

				glEnableVertexAttribArray(spBumpmapped->a("normal"));
				glVertexAttribPointer(spBumpmapped->a("normal"), 3, GL_FLOAT, false, 0, mesh.Normals.data());

				glEnableVertexAttribArray(spBumpmapped->a("tangent"));
				glVertexAttribPointer(spBumpmapped->a("tangent"), 3, GL_FLOAT, false, 0, mesh.Tangents.data());

				glEnableVertexAttribArray(spBumpmapped->a("binormal"));
				glVertexAttribPointer(spBumpmapped->a("binormal"), 3, GL_FLOAT, false, 0, mesh.Binormals.data());

				glEnableVertexAttribArray(spBumpmapped->a("texCoord"));
				glVertexAttribPointer(spBumpmapped->a("texCoord"), 2, GL_FLOAT, false, 0, mesh.TexCoords.data());

				// diffuse texture attachment to bumpmapped shader
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mesh.Texture);
				glUniform1i(spBumpmapped->u("textureMap"), 0);

				// specular texture attachment to bumpmapped shader
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mesh.Texture1);
				glUniform1i(spBumpmapped->u("textureMapSpec"), 1);

				// normalmap texture attachment to bumpmapped shader
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, mesh.Texture2);
				glUniform1i(spBumpmapped->u("textureMapBump"), 2);

				glDrawElements(GL_TRIANGLES, mesh.Indices.size(), GL_UNSIGNED_INT, mesh.Indices.data());

				// disable passed arrays for bumpmapped shader
				glDisableVertexAttribArray(spBumpmapped->a("vertex"));
				glDisableVertexAttribArray(spBumpmapped->a("normal"));
				glDisableVertexAttribArray(spBumpmapped->a("tangent"));
				glDisableVertexAttribArray(spBumpmapped->a("binormal"));
				glDisableVertexAttribArray(spBumpmapped->a("texCoord"));
			}

			if (mesh.hasBumpMap == 0)
			{
				// init and passing all matrices to non-bumpmapped shader
				spDiffuse->use();
				glUniformMatrix4fv(spDiffuse->u("P"), 1, false, glm::value_ptr(P));
				glUniformMatrix4fv(spDiffuse->u("V"), 1, false, glm::value_ptr(V));
				glUniformMatrix4fv(spDiffuse->u("M"), 1, false, glm::value_ptr(M));

				// pass phong exp. for non-bumpmapped shader
				glUniform1f(spDiffuse->u("phongExponent"), phongExponent);

				// pass shininess for non-bumpmapped shader
				glUniform1f(spDiffuse->u("shininess"), shininess);

				// pass gamma component
				glUniform1f(spDiffuse->u("gamma"), gamma);

				// pass current number of lights on scene
				glUniform1i(spDiffuse->u("lights_count"), Lights.size());

				// pass light info for all lights on scene
				for (int i = 0; i < Lights.size(); i++)
				{
					glUniform4f(spDiffuse->u(("v_light[" + std::to_string(i) + "].lightPos").c_str()), Lights.at(i).Position.x, Lights.at(i).Position.y, Lights.at(i).Position.z, 1.0f);
					glUniform4fv(spDiffuse->u(("f_light[" + std::to_string(i) + "].lightColour").c_str()), 1, glm::value_ptr(Lights.at(i).Colour));
					glUniform1f(spDiffuse->u(("f_light[" + std::to_string(i) + "].constant").c_str()), Lights.at(i).Constant);
					glUniform1f(spDiffuse->u(("f_light[" + std::to_string(i) + "].linear").c_str()), Lights.at(i).Linear);
					glUniform1f(spDiffuse->u(("f_light[" + std::to_string(i) + "].quadratic").c_str()), Lights.at(i).Quadratic);
					glUniform1f(spDiffuse->u(("f_light[" + std::to_string(i) + "].intensity").c_str()), Lights.at(i).Intensity);
				}

				// pass array data to non-bumpmapped shader
				glEnableVertexAttribArray(spDiffuse->a("vertex"));
				glVertexAttribPointer(spDiffuse->a("vertex"), 3, GL_FLOAT, false, 0, mesh.Verticies.data());

				glEnableVertexAttribArray(spDiffuse->a("normal"));
				glVertexAttribPointer(spDiffuse->a("normal"), 3, GL_FLOAT, false, 0, mesh.Normals.data());

				glEnableVertexAttribArray(spDiffuse->a("texCoord"));
				glVertexAttribPointer(spDiffuse->a("texCoord"), 2, GL_FLOAT, false, 0, mesh.TexCoords.data());

				// diffuse texture attachment to non-bumpmapped shader
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mesh.Texture);
				glUniform1i(spDiffuse->u("textureMap"), 0);

				// specular texture attachment to non-bumpmapped shader
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mesh.Texture1);
				glUniform1i(spDiffuse->u("textureMapSpec"), 1);

				glDrawElements(GL_TRIANGLES, mesh.Indices.size(), GL_UNSIGNED_INT, mesh.Indices.data());

				// disable passed arrays
				glDisableVertexAttribArray(spDiffuse->a("vertex"));
				glDisableVertexAttribArray(spDiffuse->a("normal"));
				glDisableVertexAttribArray(spDiffuse->a("texCoord"));
			}
		}
	}
};

// instantiate model
LoadedModel* model = new LoadedModel();

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 90.0f)
		fov = 90.0f;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}

void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	// *** almost all of the keys don't do anything, since anisotropic filtering is run by default ***

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			//speed_y = -PI;
		}
		if (key == GLFW_KEY_RIGHT) {
			//speed_y = PI;
		}
		if (key == GLFW_KEY_UP) {
			//speed_x = -PI;
		}
		if (key == GLFW_KEY_DOWN) {
			//speed_x = PI;
		}
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, true);
		}
		if (key == GLFW_KEY_1)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			cout << "key1 pressed" << endl;
		}
		if (key == GLFW_KEY_2)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		if (key == GLFW_KEY_3)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		if (key == GLFW_KEY_4)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		if (key == GLFW_KEY_5)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

			if (key == GLFW_KEY_LEFT_CONTROL)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		}
		if (key == GLFW_KEY_6)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		}
		if (key == GLFW_KEY_7)
		{
			float border_colour[] = { 1.0f, 1.0f, 0.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		}
		if (key == GLFW_KEY_8)
		{
			float anisoLevel = 1.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisoLevel);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisoLevel);
			cout << "aniso pressed" << endl;
		}
		if (key == GLFW_KEY_U)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		}
		if (key == GLFW_KEY_I)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		if (key == GLFW_KEY_O)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		}
		if (key == GLFW_KEY_P)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		if (key == GLFW_KEY_0)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (key == GLFW_KEY_LEFT_CONTROL)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		if (key == GLFW_KEY_GRAVE_ACCENT) //GUI access; key: "~"
		{
			if (consolePress % 2 != 0)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				glfwSetCursorPosCallback(window, NULL);
			}
			if (consolePress % 2 == 0)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				lastX = 800 / 2;
				lastY = 600 / 2;
				glfwSetCursorPosCallback(window, mouse_callback);
			}
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
			//speed_y = 0;
		}
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
			//speed_x = 0;
		}
		if (key == GLFW_KEY_GRAVE_ACCENT)
		{
			consolePress++;
		}
	}
}

void movementProcess(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
			cameraPos += cameraSpeed * cameraFront * cameraSpeedMultiply;
		else
			cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraFront * cameraSpeedMultiply;
		else
			cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * cameraSpeedMultiply;
		else
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * cameraSpeedMultiply;
		else
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

//   ***unused***
void LightMovementProcess(GLFWwindow* window)
{
	lightSpeed = 7.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
			lightPos.x += lightSpeed;
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
			lightPos.z += lightSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
			lightPos.x -= lightSpeed;
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
			lightPos.z -= lightSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		lightPos.y += lightSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		lightPos.y -= lightSpeed;
	}
}


//GLuint readTexture(const char* filename) {
//	GLuint texture;
//	//glActiveTexture(GL_TEXTURE0);
//
//	//Wczytanie do pamięci komputera
//	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
//	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
//	//Wczytaj obrazek
//	unsigned error = lodepng::decode(image, width, height, filename);
//
//	//Import do pamięci karty graficznej
//	glGenTextures(1, &texture); //Zainicjuj jeden uchwyt
//	glBindTexture(GL_TEXTURE_2D, texture); //Uaktywnij uchwyt
//	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
//	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
//		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	return texture;
//}

GLuint loadCubemap(vector<const char*> faces) 
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	unsigned width, height;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		std::vector<unsigned char> image;
		unsigned error = lodepng::decode(image, width, height, faces[i]);

		if (error)
			std::cerr << "ERROR WHILE LOADING TEXTURE. CODE: " << error << "Image: " << faces[i] << std::endl;


		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
		std::cout << "HERE: " << faces[i] << std::endl;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	cout <<"Skybox handle:"<< textureID << endl;;

	return textureID;
}

void initOpenGLProgram(GLFWwindow* window) {
    initShaders();
	cout<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<endl;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	
	// load skybox textures
	skybox_texture = loadCubemap(faces);
	
	// load model
	//bool good = model->loadModel("models/tree/forest-arch-f5.obj", true);
	bool good = model->loadModel("models/tree/project-all2-tunnel12.obj", true);

	if (!good)
		throw std::exception("Couldn't load");

	//// sound ////
	//just a background, actually useless
	//sndPlaySound(TEXT("C:\\Users\\mccv9\\source\\repos\\bio_szkielet_06_win\\sound\\wasteland_wind.wav"), SND_ASYNC | SND_LOOP);
}

void freeOpenGLProgram(GLFWwindow* window) {
	glDeleteTextures(1, &cube_texture);
	freeShaders();
}

void DrawLightSource(glm::mat4 P, glm::mat4 V, float lightSpeed)
{
	glm::mat4 light = glm::mat4(1.0f);
	light = glm::translate(light, glm::vec3(lightPosR[0].x, lightPosR[0].y, lightPosR[0].z));
	light = glm::scale(light, glm::vec3(0.3f, 0.3f, 0.3f));
	spLightSource->use();
	glUniformMatrix4fv(spLightSource->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLightSource->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(spLightSource->u("M"), 1, false, glm::value_ptr(light));
	glEnableVertexAttribArray(spLightSource->a("vertex"));
	glVertexAttribPointer(spLightSource->a("vertex"), 4, GL_FLOAT, false, 0, myCubeVertices);
	glDrawArrays(GL_TRIANGLES, 0, myCubeVertexCount);
	glDisableVertexAttribArray(spLightSource->a("vertex"));
}

void DrawSkybox(glm::mat4 P, glm::mat4 V, float* skyVerts)
{
	glm::mat4 view = glm::mat4(glm::mat3(V));
	glDepthFunc(GL_LEQUAL);
	spSkybox->use();
	glUniformMatrix4fv(spSkybox->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spSkybox->u("V"), 1, false, glm::value_ptr(view));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
	glUniform1i(spSkybox->u("skybox"), 1);
	glEnableVertexAttribArray(spSkybox->a("aPos"));
	glVertexAttribPointer(spSkybox->a("aPos"), 3, GL_FLOAT, false, 0, skyVerts);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
	glDisableVertexAttribArray(spSkybox->a("aPos"));
}

void drawScene(GLFWwindow* window,float angle_x,float angle_y) 
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// model matrix, view matrix with camera positions, projection matrix with fov and render distance variables
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 P = glm::perspective(glm::radians(fov),aspectRatio, 0.03f, renderDistance);
	M = glm::scale(M, glm::vec3(.1f, .1f, .1f));


	model->drawModel(M, P, V);

	//DrawLightSource(P, V, lightSpeed);
    DrawSkybox(P, V, skyboxVertices);
}


#pragma comment(lib, "Winmm.lib") //simplest library to use any kind of sound, which this program doesn't use, eventually
int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 4); //antialiasing

	window = glfwCreateWindow(800, 600, "Half-Life 2: Bootleg", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window)
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); //V-sync

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	cout<<glGetString(GL_VERSION)<<endl;

	initOpenGLProgram(window);

	//// ImGUI initialising ////
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	//Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// seutup Roboto
	ImFont* font = io.Fonts->AddFontFromFileTTF("fonts\\Roboto-Regular.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

	//// Main loop ////
	float angle_x = 0;
	float angle_y = 0; 
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::PushFont(font);

		//// time-dependent processing ////
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		angle_x += speed_x * deltaTime;
		angle_y += speed_y * deltaTime;
		
		//// input processing ////
		movementProcess(window);
		LightMovementProcess(window);

		//// scene draw ////
		drawScene(window,angle_x,angle_y);

		//ImGui drawing////////////////////////////////////////////////
		ImGui::Begin("Settings Panel");
		static float sliderCamSpeed = 40.0;
		if (ImGui::CollapsingHeader("Movement"))
		{
			ImGui::SliderFloat("Movement speed", &sliderCamSpeed, 0, 100);
		}
		cameraSpeed = sliderCamSpeed * deltaTime;
		static float sliderPhong = 3.0f;
		static float sliderShininess = 0.4f;
		static float sliderGamma = 1.0f;
		static float sliderRender = 3000.0f;
		if (ImGui::CollapsingHeader("Lightning"))
		{
			static int item_current = 0;
			ImGui::ListBox("Lights List", &item_current, LightsNames);
			if (ImGui::Button("Add light"))
			{
				if (Lights.size() < 10)
				{
					int l_size = Lights.size();
					std::string l_name = "Light" + std::to_string(l_size);
					cout << l_name << endl;
					Lights.emplace_back(Light("Light" + std::to_string(l_size), glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, 1.0f));
					LightsNames.emplace_back("Light" + std::to_string(l_size));
				}
			}
			if (ImGui::Button("Delete light"))
			{
				cout << item_current << endl;
				if (Lights.size() - 1 == item_current)
				{
					if (Lights.size() > 1)
					{
						cout << item_current << endl;
						Lights.erase(Lights.begin() + item_current);
						LightsNames.erase(LightsNames.begin() + item_current);
						item_current = item_current - 1;
						cout << item_current << endl;
					}
					cout << "Can't delete light; there will be no light exception" << endl;
				}
				if (item_current != Lights.size() - 1)
				{
					Lights.erase(Lights.begin() + item_current);
					LightsNames.erase(LightsNames.begin() + item_current);
				}
			}
			float sliderLightPos[3] = { Lights.at(item_current).Position.x, Lights.at(item_current).Position.y, Lights.at(item_current).Position.z };
			float sliderLightColour[3] = { Lights.at(item_current).Colour.r, Lights.at(item_current).Colour.g, Lights.at(item_current).Colour.b };
			float sliderConstant = Lights.at(item_current).Constant;
			float sliderLinear = Lights.at(item_current).Linear;
			float sliderQuadratic = Lights.at(item_current).Quadratic;
			float sliderIntensity = Lights.at(item_current).Intensity;
			ImGui::Text(Lights.at(item_current).Name.c_str());
			ImGui::SliderFloat3("Light position", sliderLightPos, -100, 100);
			Lights.at(item_current).Position.x = sliderLightPos[0];
			Lights.at(item_current).Position.y = sliderLightPos[1];
			Lights.at(item_current).Position.z = sliderLightPos[2];
			ImGui::ColorEdit3("Light Colour", sliderLightColour);
			Lights.at(item_current).Colour.r = sliderLightColour[0];
			Lights.at(item_current).Colour.g = sliderLightColour[1];
			Lights.at(item_current).Colour.b = sliderLightColour[2];
			ImGui::SliderFloat("Phong exponent", &sliderPhong, 0, 512);
			phongExponent = sliderPhong;
			ImGui::SliderFloat("Shininess", &sliderShininess, -100, 100);
			shininess = sliderShininess;
			ImGui::SliderFloat("Constant", &sliderConstant, 0, 1);
			Lights.at(item_current).Constant = sliderConstant;
			ImGui::SliderFloat("Linear", &sliderLinear, 0, 1);
			Lights.at(item_current).Linear = sliderLinear;
			ImGui::SliderFloat("Quadratic", &sliderQuadratic, 0, 1);
			Lights.at(item_current).Quadratic = sliderQuadratic;
			ImGui::SliderFloat("Intensity", &sliderIntensity, 0, 100);
			Lights.at(item_current).Intensity = sliderIntensity;
		}
		if (ImGui::CollapsingHeader("Rendering"))
		{
			ImGui::SliderFloat("Render Distance", &sliderRender, 0, 10000);
			renderDistance = sliderRender;
		}
		if(ImGui::CollapsingHeader("Post-processing"))
		{
			ImGui::SliderFloat("Gamma", &sliderGamma, -10, 10);
			gamma = sliderGamma;
		}
		ImGui::End();

		///// next window /////

		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 2, main_viewport->WorkPos.y + 2));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		ImGui::Begin("Overlay Test", NULL, window_flags);
		ImGui::Text("Half-Life 2 bootleg\nPress \"~\" to activate/deactivate GUI");
		ImGui::Text("%.3f ms/frame %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		ImGui::PopFont();

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		///ImGui end///////////////////////////////////////////////////////

		glfwSwapBuffers(window);
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
