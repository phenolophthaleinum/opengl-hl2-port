//#version 330
//
//layout(location = 0) in vec3 aPos;
//layout(location = 1) in vec3 aNormal;
//layout(location = 2) in vec2 aTexCoords;
//
//out vec3 FragPos;
//out vec3 Normal;
//out vec2 TexCoords;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//void main()
//{
//    FragPos = vec3(model * vec4(aPos, 1.0));
//    Normal = mat3(transpose(inverse(model))) * aNormal;
//    TexCoords = aTexCoords;
//
//    gl_Position = projection * view * vec4(FragPos, 1.0);
//}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#version 330

//#define LIGHTS_NUM 1

//uniform Light {
//    vec4 lightPos;
//    float intensity;
//}light;

//struct VLight {
//    vec4 lightPos;
//};

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//uniform vec4 lightPos;
//uniform VLight v_light[LIGHTS_NUM];
//uniform int hasBumpMap;

//uniform mat4 lightSpaceMatrix;

//Atrybuty
layout(location = 0) in vec3 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
layout(location = 1) in vec3 normal; //wektor normalny w wierzcholku
layout(location = 2) in vec2 texCoord; //wspó³rzêdne teksturowania
//in vec3 tangent;
//in vec3 binormal;

//Zmienne interpolowane
out vec2 iTex;
out vec2 iTex1;
//out vec3 FragPos;
//out vec3 Normal;
out vec3 n;
//out vec4 l;
out vec3 v;
//out vec4 lightSpace;
//out int hasBump;
//out float distance;

//out Light{
//    vec4 lightPos;
//    float intensity;
//}outLight;

//out mat4 Vfrag;
//out mat4 Mfrag;
//out vec3 vertexfrag;

//float CalculateDistance(VLight light)
//{
//    return length(light.lightPos - M * vec4(vertex, 1.0f));
//}
//
//vec4 CalculateL(VLight light)
//{
//    return normalize(V * light.lightPos - V * M * vec4(vertex, 1.0f));
//}

void main(void) {
    //OLD

    /*FragPos = vec3(M * vec4(vertex, 1.0));
    Normal = mat3(transpose(inverse(M))) * normal;
    i_texCord = texCoord;
    gl_Position = P * V * vec4(FragPos, 1.0);*/

    //////////////////

    //NEW
    /*if (hasBumpMap == 0)
    {*/
    //calculating pos in vertexshader
    //l = normalize(V * light.lightPos - V * M * vec4(vertex, 1.0f));

    /*Vfrag = V;
    Mfrag = M;
    vertexfrag = vertex;*/

    n = mat3(transpose(inverse(M))) * normal;

    v = vec3(M * vec4(vertex, 1.0f));

    //lightSpace = lightSpaceMatrix * l;

    iTex = texCoord;

    /*for (int i = 0; i < LIGHTS_NUM; i++)
    {
        l = CalculateL(v_light[i]);
        distance = CalculateDistance(v_light[i]);
    }*/

    //iTex1 = (n.xy + 1) / 2;

    //attenuation
    //distance = length(light.lightPos - M * vec4(vertex, 1.0f));

    //outLight = light;
    /*outLight.lightPos = light.lightPos;
    outLight.intensity = light.intensity;*/

    gl_Position = P * V * vec4(v, 1.0f);
    /*  }*/

      ///////////////////

      //NEW + normalmap
     /* if (hasBumpMap == 1)
      {*/
      /*mat4 TBN = mat4(vec4(tangent, 0), vec4(binormal, 0), vec4(normal, 0), vec4(0, 0, 0, 1));
      mat4 invTBN = inverse(TBN);

      l = normalize(invTBN * inverse(M) * lightPos - invTBN * vec4(vertex, 1.0f));

      v = normalize(invTBN * inverse(V * M) * vec4(0, 0, 0, 1) - invTBN * vec4(vertex, 1.0f));



      iTex = texCoord;


      gl_Position = P * V * M * vec4(vertex, 1.0f);*/
      /*  }*/
}
