#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;

uniform float _time;
uniform float _scale;
uniform vec2 _minOffset;
uniform vec2 _maxOffset;

out vec2 uv;

void main()
{
	uv = vUV;

	float loopTime = sin(_time) * 0.5 + 0.5;
	float loopTime2 = abs(sin(_time * 2.0));

	vec2 animOffset = mix(_minOffset + vec2(_scale), _maxOffset - vec2(_scale), vec2(loopTime, loopTime2));

	gl_Position = vec4(vPos * _scale + vec3(animOffset, 1.0), 1.0);
}