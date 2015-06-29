#version 430

in vec3 Position;
in vec3 Normal;

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
};
uniform MaterialInfo Material;

const int levels = 3;
const float scaleFactor = 1.0 / levels;

layout (location = 0) out vec4 FragColor;

vec3 toonShade()
{
	vec3 s = normalize(Light.Position.xyz - Position.xyz);
	vec3 ambient = Material.Ka;
	float cosine= dot(s, Normal);
	vec3 diffuse= Material.Kd* floor(cosine * levels) * scaleFactor;
	
	return Light.Intensity * (ambient + diffuse);
}

void main()
{
	FragColor = vec4(toonShade(), 1.0);
}