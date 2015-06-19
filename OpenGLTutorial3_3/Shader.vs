#version 430

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 Color;

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Lights[3];

struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;
};
uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

vec3 ads(int lightIndex, vec4 position, vec3 norm)
{
	vec3 s= normalize(vec3(Lights[lightIndex].Position - position));
	
	vec3 v= normalize(vec3(-position));
	vec3 r = reflect(-s, norm);
	vec3 i= Lights[lightIndex].Intensity;
	
	return i * (Material.Ka + 
			Material.Kd * max(dot(s, norm), 0.0) +
			Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess));
}

void main()
{
	vec3 eyeNorm = normalize(NormalMatrix * VertexNormal);
	vec4 eyePosition = ModelViewMatrix * VertexPosition;
	
	Color= vec3(0.0);
	
	for(int i = 0; i < 3; i++)
	{
		Color += ads(i, eyePosition, eyeNorm);
	}
	
	gl_Position = MVP * VertexPosition;
}