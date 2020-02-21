#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform vec3 dCol;
uniform vec3 aCol;
uniform vec3 sCol;
uniform vec3 static_dCol;
uniform vec3 static_aCol;
uniform vec3 static_sCol;

out vec3 diffuseColor;
out vec3 ambColor;
out vec3 specColor; 
out vec3 staticDiffuseColor;
out vec3 staticAmbColor;
out vec3 staticSpecColor; 


// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;
uniform vec3 lightPosWorldSpace;


out vec3 Position_worldspace;
out vec3 normalCameraSpace;
flat out vec3 flat_Normal_cameraspace;
out vec3 viewDirerctionCameraspace;
out vec3 lightDirectionCameraspace;

flat out vec3 fColor;


void main(){
	Position_worldspace = (M * vec4(position,1)).xyz;



	vec3 vertexPosition_cameraspace = ( V * M * vec4(position,1)).xyz;
	viewDirerctionCameraspace = vec3(0,0,0) - vertexPosition_cameraspace;


	vec3 LightPosition_cameraspace = ( V * vec4(lightPosWorldSpace,1)).xyz;
	lightDirectionCameraspace = LightPosition_cameraspace + viewDirerctionCameraspace;


	normalCameraSpace = vec3( V * M * vec4(normal,0)); 
	flat_Normal_cameraspace = normalCameraSpace;



	diffuseColor = dCol;
	ambColor = aCol;
	specColor = sCol;	
	staticDiffuseColor = static_dCol;
	staticAmbColor = static_aCol;
	staticSpecColor = static_sCol;
	gl_Position = MVP * vec4(Position_worldspace, 1.0f);


}