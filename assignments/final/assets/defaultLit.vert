/* 
* Modified by Adam Gyenes
* Fork of assignment7
*/

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
	mat3 tbn;
} vs_out;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

void main()
{
	vec3 t = normalize(mat3(_Model) * vTangent);
	vec3 b = normalize(mat3(_Model) * vBitangent);
	vec3 n = normalize(mat3(_Model) * vNormal);
	mat3 tbn = transpose(mat3(t, b, n));

	vs_out.position = mat3(_Model) * vPos;
	vs_out.normal = n;
	vs_out.tangent = t;
	vs_out.bitangent = b;
	vs_out.UV = vUV;
	vs_out.tbn = tbn;

	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}