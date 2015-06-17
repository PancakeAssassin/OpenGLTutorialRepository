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


void main()
{
	vec3 tnorm = normalize(NormalMatrix * VertexNormal);
	vec4 eyeCoords = ModelViewMatrix * VertexPosition;
	vec3 s = normalize(vec3(Light.Position - eyeCoords));
	vec3 v = normalize(-eyeCoords.xyz);
	vec3 r = reflect(-s, tnorm);
	vec3 ambient= Light.La * Material.Ka;
	float sDotN = max(dot(s, tnorm), 0.0);
	vec3 diffuse = Light.Ld * Material.Kd * sDotN;
	vec3 spec = vec3(0.0);
	if(sDotN > 0.0)
	{
		spec = Light.Ls * Material.Ks *
			pow(max(dot(r,v), 0.0), Material.Shininess);
	}
	VFragColor = ambient + diffuse + spec;
	
	gl_Position= MVP * VertexPosition;
}