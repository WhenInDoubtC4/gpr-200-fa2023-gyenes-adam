#version 450

#define MAX_LIGHTS 4

struct Light
{
	vec3 position;
	vec3 color;
};

in Surface
{
	vec3 position;
	vec3 normal;
	vec2 UV;
} fs_in;

uniform vec3 _cameraPosition;
uniform sampler2D _Texture;
uniform vec3 _ambientColor = vec3(0.341, 0.365, 0.51);
uniform Light _lights; //Keep this 0d for now

uniform float _ambientK = 0.2;
uniform float _diffuseK = 0.7;
uniform float _specularK = 0.5;
uniform float _shininess = 0.3;

out vec4 FragColor;

void main()
{
	vec3 lightDirection = normalize(_lights.position - fs_in.position); //omega
	vec3 reflected = reflect(-lightDirection, fs_in.normal); //r
	vec3 camera = normalize(_cameraPosition - fs_in.position); //v
	vec3 halfVec = normalize(lightDirection + camera); //h

	//Blinn-phong
	vec3 ambient = _ambientColor * _ambientK;
	vec3 diffuse = _lights.color * _diffuseK * max(dot(fs_in.normal, lightDirection), 0.0);
	vec3 specular = _lights.color * _diffuseK * pow(max(dot(halfVec, fs_in.normal), 0.0), _shininess);

	vec4 texColor = texture(_Texture, fs_in.UV);

	vec3 light;
	light += ambient;
	light += diffuse;
	light += specular;

	texColor *= vec4(light, 0.0);

	FragColor = texColor;
}