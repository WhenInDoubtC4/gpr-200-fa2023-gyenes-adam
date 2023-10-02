#version 450

in vec2 uv;

uniform sampler2D _backgroundTexture;
uniform sampler2D _noiseTexture;
uniform float _time;
uniform vec3 _waterColor;
uniform float _waveSpeed;
uniform float _warpIntenisty;

out vec4 FragColor;

void main()
{
	vec2 noiseUV =  uv + fract(_time * _waveSpeed);
	vec2 warpedUV = uv * mix(vec2(1.0), texture(_noiseTexture, noiseUV).rg, _warpIntenisty);

	vec4 background = texture(_backgroundTexture, warpedUV);

	FragColor = background * vec4(_waterColor, 1.0);
}