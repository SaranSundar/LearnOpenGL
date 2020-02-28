#version 330 core

out vec4 result_color;

uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 object_color;
uniform vec3 view_position;
uniform bool colour;
uniform bool flag;
uniform bool lights;
uniform bool normalcol;
uniform bool greyscale;
uniform bool red;
uniform bool blue;
uniform bool green;
uniform int shinyness;
uniform bool isSmooth;
uniform bool isPosLightOn;

uniform vec3 posAmbientLightColor;
uniform vec3 posDiffuseLightColor;
uniform vec3 posSpecularLightColor;

flat in vec3 fragment_position; //interpolated
flat in vec3 normal;

smooth in vec3 fragment_positionS; //interpolated
smooth in vec3 normalS;

flat in vec3 col;

void calculateSmooth(vec3);
void calculateFlat(vec3);
vec3 calculatePosLight(vec3, vec3, vec3);

void main()
{
	vec3 lightColor = light_color;
	vec3 posLight;
	if(!isPosLightOn){
		lightColor = vec3(0, 0, 0);
	}
	if(isSmooth){
		posLight = calculatePosLight(lightColor, fragment_positionS, normalS);
	}
	else{
		posLight = calculatePosLight(lightColor, fragment_position, normal);
	}

	result_color = vec4((posLight ) * object_color, 1.0f);
} 

vec3 calculatePosLight(vec3 light_color, vec3 fragment_position, vec3 normal){
	//ambient
	float ambient_strength = 0.25f;
	vec3 ambient = ambient_strength * posAmbientLightColor;

	//diffuse
	vec3 light_direction = normalize(light_position - fragment_position);
	float diffuse_strength = 0.75f;  //use max so that it doesn't go negative
	vec3 diffuse = diffuse_strength * max(dot(normalize(normal), light_direction), 0.0) * posDiffuseLightColor;

	//Specular
	vec3 view_direction = normalize(view_position - fragment_position);
	vec3 reflect_light_direction = reflect(-light_direction, normalize(normal));
	float specular_strength = 1.0f;
	vec3 specular = specular_strength * pow(max(dot(reflect_light_direction, view_direction), 0.0), shinyness) * posSpecularLightColor;
	
	vec3 color = (specular + diffuse + ambient);
	return color;
}
