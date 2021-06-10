#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;


//uniform vec4 lightDir=vec4(2,2,2,0);
uniform vec4 lightDir;
uniform vec3 viewPos;

//Atrybuty
layout (location=0) in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
layout (location=1) in vec4 normal; //wektor normalny w wierzcholku
layout (location=2) in vec2 texCoord; //wspó³rzêdne teksturowania


//Zmienne interpolowane
out vec2 i_tc;
out float i_nl;
out float spec;

void main(void) {
    vec4 FragPos = M * vertex;
    //gl_Position=P*V*M*vertex;
    //gl_Position = P * V * M * vec4(FragPos, 1.0);
    gl_Position = P * V * FragPos;

    vec4 lightDir2 = normalize(lightDir - FragPos);
    mat4 G=mat4(transpose(inverse(mat3(M))));
    //vec4 n=normalize(V*G*normal);
    vec4 n = normalize(G * normal);

    vec4 viewDir = normalize(vec4(viewPos, 1.0) - FragPos);
    vec4 reflectDir = reflect(-lightDir, n);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    //vec4 specularComponent = specularStrength * spec;

    //i_nl=clamp(dot(n,lightDir),0,1);
    //i_nl = max(dot(n, lightDir2), 0.0);
    i_nl=clamp(dot(n,lightDir),0,1);
    i_tc=texCoord;
}
