#version 430

in vec3 VFragColor;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(VFragColor, 1.0);
}