#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUvCoord;

out vec2 uv;

void main()
{
	uv = vUvCoord;
	gl_Position = vec4(vPos, 1.0);
}