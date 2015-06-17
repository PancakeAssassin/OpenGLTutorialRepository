#version 430

in vec3 VertexPosition;
in vec3 VertexColor;

out vec3 VFragColor;

uniform mat4 MVP;

void main()
{
	VFragColor= VertexColor;
	vec4 v= vec4(VertexPosition, 1);
	gl_Position= MVP * v;
}