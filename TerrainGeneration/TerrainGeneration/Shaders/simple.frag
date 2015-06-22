#version 430

in vec3 Position;

layout (binding = 0) uniform sampler2D Tex1;

layout (location = 0) out vec4 FragColor;

void main()
{
	//denoms will be replaced with texWidth and texHeight uniforms upon success
	vec2 TexCoord= vec2(Position.x/1024, Position.z/1024);
	
	FragColor= texture(Tex1, TexCoord);
}