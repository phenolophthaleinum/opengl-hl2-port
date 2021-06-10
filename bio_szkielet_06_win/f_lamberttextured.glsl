#version 330


uniform sampler2D tex;
uniform vec4 lightColor;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

//Zmienne interpolowane
in float i_nl;
in vec2 i_tc;
in float spec;

void main(void) {
	vec4 color = texture(tex, i_tc);
	float ambientStrength = 0.1;
	//vec4 ambient = ambientStrength * vec4(vec3(0.9, 0.9, 0.9), 1.0);
	vec4 ambient = ambientStrength * lightColor * color;
    //vec4 color=texture(tex,i_tc);
	//vec4 diffuse = i_nl * vec4(vec3(1, 1, 1), 1.0);
	vec4 diffuse = i_nl * lightColor * color;
	
	float specularStrength = 0.9;
	vec4 specular = (specularStrength * spec * color) * lightColor;

	//vec4 diffuse = vec4(color.rgb * i_nl, color.a);
	//vec4 ambient = diffuse * vec4(vec3(0.1, 0.1, 0.1), 1.0);
	//pixelColor = (diffuse + ambient);
	//pixelColor = (diffuse + ambient) * vec4(vec3(.9, .9, .9), 1.0);
	//pixelColor = (diffuse + ambient) * color;
	//pixelColor = (diffuse + ambient + specular) * color;
	pixelColor = diffuse + ambient + specular;
}
