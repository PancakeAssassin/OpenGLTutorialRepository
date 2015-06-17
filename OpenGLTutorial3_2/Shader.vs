#version 430

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 VPosition;
out vec3 VNormal;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

void main()
{
	VNormal = normalize(NormalMatrix * VertexNormal);
	VPosition = vec3(ModelViewMatrix * VertexPosition);
	gl_Position = MVP * VertexPosition;
}