#version 450

in vec2 uv;

uniform sampler2D _texture;

out vec4 FragColor;

void main()
{
	FragColor = texture(_texture, uv);
}