#version 430

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 VFragColor;

struct LightInfo
{
	vec4 Position;
	vec3 La; 
	vec3 Ld;
	vec3 Ls;
};
uniform LightInfo Light;

struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;
};
uniform MaterialInfo Material;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

vec3 ads(vec4 position, vec3 norm)
{
	vec3 s;
	if(Light.Position.w == 0.0)
		s = normalize(vec3(Light.Position));
	else
		s = normalize(vec3(Light.Position - position));
	
	vec3 v = normalize(vec3(-position));
	vec3 r = reflect(-s, norm);
	
	return
		(Light.La * Material.Ka +
		Light.Ld * Material.Kd * max(dot(s, norm), 0.0) +
		Light.Ls * Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess));
}


void main()
{
	vec3 norm = normalize(NormalMatrix * VertexNormal);
	vec4 eyePosition = ModelViewMatrix * VertexPosition;
	
	VFragColor = ads(eyePosition, norm);
	
	gl_Position= MVP * VertexPosition;
}