#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec3 vBitangent;
layout(location = 4) in vec2 vUV;

out Surface
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	vec2 UV;
} vs_out;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

void main()
{
	vs_out.position = mat3(_Model) * vPos;
	vs_out.normal = mat3(_Model) * vNormal;
	vs_out.tangent = mat3(_Model) * vTangent;
	vs_out.bitangent = mat3(_Model) * vBitangent;
	vs_out.UV = vUV;

	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}