#version 450

in vec2 uv;

uniform vec3 _Color;
uniform float _Brightness;

out vec4 FragColor;

void main()
{
	FragColor = vec4(uv, 0.0, 1.0);
}