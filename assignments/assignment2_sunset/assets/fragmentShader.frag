#version 450

uniform vec3 _Color;
uniform float _Brightness;

out vec4 FragColor;

void main()
{
	FragColor = vec4(_Color * _Brightness, 1.0);
}