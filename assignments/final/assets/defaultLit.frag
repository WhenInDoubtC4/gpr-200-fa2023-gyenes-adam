/* 
* Modified by Adam Gyenes
* Fork of assignment7
*/


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
uniform sampler2D _colorTexture;
uniform sampler2D _heightTexture;
uniform Material _material;
uniform vec3 _ambientColor;
uniform int _activeLights;
uniform Light _lights[MAX_LIGHTS];

uniform int _parallaxMethod;
uniform int _discardOutOfBoundFrags;
uniform float _heightScale;
uniform float _minLayers;
uniform float _maxLayers;

out vec4 FragColor;

//https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
vec2 SimpleParallaxMapping(vec2 UV, vec3 viewDir)
{
	float height = texture(_heightTexture, UV).r;

	vec2 p = viewDir.xy / viewDir.z * (height * _heightScale);
	return UV - p;
}

vec2 SteepParallaxMapping(vec2 UV, vec3 viewDir)
{
	float numLayers = mix(_maxLayers, _minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
	
	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;

	vec2 p = viewDir.xy * _heightScale;
	vec2 deltaTexCoords = p / numLayers;
	vec2 currentUV = UV;
	float height = texture(_heightTexture, currentUV).r;
	while (currentLayerDepth < height)
	{
		currentUV -= deltaTexCoords;
		height = texture(_heightTexture, currentUV).r;
		currentLayerDepth += layerDepth;
	}

	return currentUV;
}

vec2 ParallaxOcclusionMapping(vec2 UV, vec3 viewDir)
{
	float numLayers = mix(_maxLayers, _minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
	
	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;

	//vec2 p = viewDir.xy / viewDir.z * (height * _heightScale);
	vec2 p = viewDir.xy * _heightScale;
	vec2 deltaUV = p / numLayers;
	vec2 currentUV = UV;
	float height = texture(_heightTexture, currentUV).r;
	while (currentLayerDepth < height)
	{
		currentUV -= deltaUV;
		height = texture(_heightTexture, currentUV).r;
		currentLayerDepth += layerDepth;
	}

	//Copy of steep parallax mapping code above

	vec2 prevUV = currentUV + deltaUV;

	float afterHeight = height - currentLayerDepth;
	float beforeHeight = texture(_heightTexture, prevUV).r - currentLayerDepth + layerDepth;

	float weight = afterHeight / (afterHeight - beforeHeight);
	vec2 finalUV = prevUV * weight + currentUV * (1.0 - weight);

	return finalUV;
}

void main()
{
	vec3 camera = normalize(_cameraPosition - fs_in.position); //v

	vec3 ambient = _ambientColor * _material.ambientK;
	vec3 light = ambient;

	//vec3 tangentLightPos = fs_in.tbn * _lights[0].position;
	vec3 tangentViewPos = fs_in.tbn * _cameraPosition;
	vec3 tangentFragPos = fs_in.tbn * fs_in.position;
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);

	//Parallax method
	vec2 finalUV;
	if (_parallaxMethod == 0) finalUV = fs_in.UV;
	else if (_parallaxMethod == 1) finalUV = SimpleParallaxMapping(fs_in.UV, viewDir);
	else if (_parallaxMethod == 2) finalUV = SteepParallaxMapping(fs_in.UV, viewDir);
	else finalUV = ParallaxOcclusionMapping(fs_in.UV, viewDir);

	//Discard out of bound frags
	if(_discardOutOfBoundFrags == 1 && (finalUV.x > 1.0 || finalUV.y > 1.0 || finalUV.x < 0.0 || finalUV.y < 0.0)) discard;

	//Lighting
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

	vec4 texColor = texture(_colorTexture, finalUV);
	texColor *= vec4(light, 0.0);

	FragColor = texColor;

	//FragColor = vec4(fs_in.tangent, 0.0);
}
