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
	mat3 tbn;
} fs_in;

uniform vec3 _cameraPosition;
uniform sampler2D _Texture;
uniform sampler2D _heightTexture;
uniform Material _material;
uniform vec3 _ambientColor;
uniform int _activeLights;
uniform Light _lights[MAX_LIGHTS];

const float height_scale = 0.1;
	
const float minLayers = 8.0;
const float maxLayers = 32.0;

out vec4 FragColor;

//https://learnopengl.com/Advanced-Lighting/Parallax-Mapping

vec2 BasicParallaxMapping(vec2 texCoords, vec3 viewDir)
{
	float height = texture(_heightTexture, texCoords).r;
	vec2 p = viewDir.xy / viewDir.z * (height * height_scale);

	return texCoords - p;
}

vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir)
{
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
	
	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;

	//vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
	vec2 p = viewDir.xy * height_scale;
	vec2 deltaTexCoords = p / numLayers;
	vec2 currentTexCoords = texCoords;
	float height = texture(_heightTexture, currentTexCoords).r;
	while (currentLayerDepth < height)
	{
		currentTexCoords -= deltaTexCoords;
		height = texture(_heightTexture, currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}

	return currentTexCoords;
}

vec2 ParallaxOcclusionMapping(vec2 texCoords, vec3 viewDir)
{
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
	
	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;

	//vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
	vec2 p = viewDir.xy * height_scale;
	vec2 deltaTexCoords = p / numLayers;
	vec2 currentTexCoords = texCoords;
	float height = texture(_heightTexture, currentTexCoords).r;
	while (currentLayerDepth < height)
	{
		currentTexCoords -= deltaTexCoords;
		height = texture(_heightTexture, currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}

	//Copy of steep parallax mapping code above

	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	float afterHeight = height - currentLayerDepth;
	float beforeHeight = texture(_heightTexture, prevTexCoords).r - currentLayerDepth + layerDepth;

	float weight = afterHeight / (afterHeight - beforeHeight);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

void main()
{
	vec3 camera = normalize(_cameraPosition - fs_in.position); //v

	vec3 ambient = _ambientColor * _material.ambientK;
	vec3 light = ambient;

	vec3 tangentLightPos = fs_in.tbn * _lights[0].position;
	vec3 tangentViewPos = fs_in.tbn * _cameraPosition;
	vec3 tangentFragPos = fs_in.tbn * fs_in.position;
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
	vec2 texCoords = ParallaxOcclusionMapping(fs_in.UV, viewDir);
	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) discard;

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

	float displ = texture(_heightTexture, texCoords).r;

	texColor *= vec4(light, 0.0);

	//FragColor = texColor;

	FragColor = vec4(displ, 0.0, 0.0, 0.0);
}
