#version 430

in vec3 VPosition;
in vec3 VNormal;

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

layout (location = 0) out vec4 FragColor;

vec3 Phong()
{
	vec3 n = normalize(VNormal);
	vec3 s = normalize(vec3(Light.Position) - VPosition);
	vec3 v = normalize(vec3(-VPosition));
	vec3 r = reflect(-s, n);
	
	return (Light.La * Material.Ka) + 
			(Light.Ld * Material.Kd * max(dot(s,n), 0.0)) +
			(Light.Ls * Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess));			
}

void main()
{
	FragColor = vec4(Phong(), 1.0);
}