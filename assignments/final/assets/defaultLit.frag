#version 450

#define MAX_LIGHTS 4

struct Light
{
	vec3 position;
	vec3 color;
};

struct Material
{
	float ambientK;
	float diffuseK;
	float specularK;
	float shininess;
};

in Surface
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	vec2 UV;
} fs_in;

uniform vec3 _cameraPosition;
uniform sampler2D _Texture;
uniform Material _material;
uniform vec3 _ambientColor;
uniform int _activeLights;
uniform Light _lights[MAX_LIGHTS];

out vec4 FragColor;

void main()
{
	vec3 camera = normalize(_cameraPosition - fs_in.position); //v

	vec3 ambient = _ambientColor * _material.ambientK;
	vec3 light = ambient;

	for (int i = 0; i < _activeLights; i++)
	{
		vec3 lightDirection = normalize(_lights[i].position - fs_in.position); //omega
		//vec3 reflected = reflect(-lightDirection, normalize(fs_in.normal)); //r
		vec3 halfVec = normalize(lightDirection + camera); //h

		//Blinn-phong
		vec3 diffuse = _lights[i].color * _material.diffuseK * max(dot(normalize(fs_in.normal), lightDirection), 0.0);
		vec3 specular = _lights[i].color * _material.specularK * pow(max(dot(halfVec, normalize(fs_in.normal)), 0.0), _material.shininess);

		light += diffuse;
		light += specular;
	}

	vec4 texColor = texture(_Texture, fs_in.UV);

	texColor *= vec4(light, 0.0);

	//FragColor = texColor;

	FragColor = vec4(fs_in.tangent, 0.0);
}