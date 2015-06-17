#version 430 

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

uniform vec4 LightPosition;	//light position
uniform vec3 Ld;			//light intensity
uniform vec3 Kd; 			//diffuse reflectivity

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	vec3 tnorm= normalize(NormalMatrix * VertexNormal);
	vec4 eyeCoords = ModelViewMatrix * VertexPosition;
	
	vec3 s= normalize(vec3(LightPosition - eyeCoords));
	
	LightIntensity = Ld * Kd * max(dot(s, tnorm), 0.0);
	
	gl_Position= MVP * VertexPosition;
}