#version 330 core
out vec4 fragColor;



in vec3 diffuseColor;
in vec3 ambColor;
in vec3 specColor;
in vec3 staticDiffuseColor;
in vec3 staticAmbColor;
in vec3 staticSpecColor;




// Interpolated values from the vertex shaders
in vec3 Position_worldspace;
in vec3 normalCameraSpace;
flat in vec3 flat_Normal_cameraspace;
in vec3 viewDirerctionCameraspace;
in vec3 lightDirectionCameraspace;
flat in vec3 fColor;


uniform mat4 MV;
uniform vec3 lightPosWorldSpace;

uniform bool isDyOn;
uniform bool isStOn;
uniform bool isFlat;
uniform vec3 modCol;
uniform int shininess;

void main(){

	vec3 LightColor = vec3(1,1,1);
	vec3 StaticLightPos = vec3(0,0,0);
	float LightPower = 75.0f;
	
	vec3 MaterialDiffuseColor = diffuseColor;
	vec3 MaterialAmbientColor = ambColor;
	vec3 MaterialSpecularColor = specColor;	

	vec3 StaticMaterialDiffuseColor = staticDiffuseColor;
	vec3 StaticMaterialAmbientColor = staticAmbColor;
	vec3 StaticMaterialSpecularColor = staticSpecColor;




	float distance = length( lightPosWorldSpace - Position_worldspace );
	float distanceFixed = length( StaticLightPos - Position_worldspace );

	vec3 n = normalize( normalCameraSpace );
	if(isFlat)
	{
		n = normalize( flat_Normal_cameraspace );
	}
	vec3 l = normalize( lightDirectionCameraspace );


	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	vec3 E = normalize(viewDirerctionCameraspace);
	vec3 R = reflect(-l,n);
	vec3 halfwayDir = normalize(l + E);  
	float cosAlpha = max(dot(n, halfwayDir), 0.0);



    vec3 ambient = staticAmbColor;


    vec3 lightDir = normalize(-vec3(0.0f,-1.0f,-1.0f));  
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = staticDiffuseColor * diff;


	//static specular
    vec3 viewDir = E;
	halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(n, halfwayDir), 0.0), shininess);
    vec3 specular = staticSpecColor * spec;
	int d = 0;
	int s = 0;
	if(isDyOn)
	{
		d = 1;
	}
	if(isStOn)
	{
		s = 1;
	}





	vec4 dynamicLight = vec4( (MaterialAmbientColor + (MaterialDiffuseColor * LightPower  * cosTheta / (distance*distance)) + (MaterialSpecularColor * LightPower  * pow(cosAlpha,shininess) / (distance*distance))), 1.0f);
	vec3 statV3 = ambient + diffuse + specular;
	vec4 staticLight = vec4(statV3, 1.0f);
	vec4 modColor = vec4(modCol, 1.0f);
	fragColor = modColor + (dynamicLight * d) + (staticLight * s) ;
}