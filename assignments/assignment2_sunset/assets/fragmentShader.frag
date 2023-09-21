#version 450

in vec2 uv;

uniform vec2 _resolution;
uniform float _time;
uniform float _timeScale;

uniform vec3 _dayHorizonColor;
uniform vec3 _dayZenithColor;
uniform vec3 _nightHorizonColor;
uniform vec3 _nightZenithColor;

uniform vec2 _sunPos;
uniform vec3 _sunColor;
uniform float _sunRadius;
uniform float _sunSmoothness;
uniform float _sunFlareRadius;
uniform float _sunFlareSmoothness;
uniform float _sunFlareIntensity;

uniform vec2 _moonPos;
uniform vec3 _moonColor;
uniform float _moonRadius;
uniform float _moonSmoothness;

uniform vec3 _hillsL1Color;
uniform vec3 _hillsL2Color;
uniform vec3 _treesColor;
uniform float _shadowDarkness;
uniform float _shadowIntensity;
uniform float _shadowLength;

out vec4 FragColor;

void main()
{
    //UV matched to screen aspect ratio (0.0 in center)
    float aspectRatio = _resolution.x / _resolution.y;
    vec2 screenUv = (uv * 2.0 - 1.0) * vec2(aspectRatio, 1.0);

    //Time normalized between 0-1
    float normTime = sin(_timeScale * _time) * 0.5 + 0.5;

    //Background
    vec3 dayBackground = mix(_dayHorizonColor, _dayZenithColor, uv.y);
    vec3 nightBackground = mix(_nightHorizonColor, _nightZenithColor, uv.y);
    //Day and night cycle background
    vec3 animBackground = mix(nightBackground, dayBackground, normTime);

    //Sun
    //Animated sun position
    vec2 animSunPos = mix(vec2(_sunPos.x, -1.0), _sunPos, normTime);
    //Sun circle
    float sunMask = 1.0 - smoothstep(_sunRadius - _sunSmoothness, _sunRadius + _sunSmoothness, distance(screenUv, animSunPos));
    //Halo effect around sun
    float sunFlareMask = 1.0 - smoothstep(_sunFlareRadius - _sunFlareSmoothness, _sunFlareRadius + _sunFlareSmoothness, distance(screenUv, animSunPos));
    sunFlareMask *= _sunFlareIntensity;
    
    //Moon
    //Moon circle
    float moonMain = 1.0 - smoothstep(_moonRadius - _moonSmoothness, _moonRadius + _moonSmoothness, distance(screenUv, _moonPos));
    //Offset circle
    float moonCutout = 1.0 - smoothstep(_moonRadius - _moonSmoothness, _moonRadius + _moonSmoothness, distance(screenUv, _moonPos + vec2(_moonRadius, _moonRadius) * 0.5));
    //Crescent shape (using cutout mask)
    float moonMask = clamp(moonMain - moonCutout, 0.0, 1.0);
    moonMask = mix(moonMask, 0.0, normTime);
    
    //Place colorized sun and moon on background
    float sunCombined = max(sunMask, sunFlareMask);
    vec3 final = mix(animBackground, _sunColor, sunCombined);
    final = mix(final, _moonColor, moonMask);

    //Hills
    //Scaled function inputs
    float hx = uv.x * 6.0;
    float hx2 = uv.x * 9.0;

    //Shadow length to follow day/night cycle
    float animShadowLength = mix(0.0, _shadowLength, normTime);

    //Some arbitrary sine function to generate hill/mountain shape
    float hillsL1 = abs(sin(hx) + 0.5 * sin(2.0 * hx + 0.5) + 0.3 * sin(3.0 * hx + 0.7) + 0.3 * sin(4.0 * hx + 1.4)) * 0.25 + 0.1;
    //Cutout mask (m) and shadow (s)
    float hillsL1m = 1.0 - step(hillsL1, uv.y);
    float hillsL1s = smoothstep(hillsL1 - animShadowLength, hillsL1, uv.y);
    hillsL1s -= 1.0 - hillsL1m;
    hillsL1s *= _shadowIntensity;
    
    //Repeat with secondary set of hills and trees
    float hillsL2 = abs(0.4 * sin(2.0 * hx2) + 0.2 * sin(3.0 * hx2) + 2.0 * sin(4.0 * hx2)) * 0.05 + 0.1;
    float hillsL2m = 1.0 - step(hillsL2, uv.y);
    float hillsL2s = smoothstep(hillsL2 - animShadowLength, hillsL2, uv.y);
    hillsL2s -= 1.0 - hillsL2m;
    hillsL2s *= _shadowIntensity;

    //Arbitrary "sawtooth"/triangle function
    float trees = abs(fract(hx2) + 2.0 * fract(2.0 * hx2) + 6.0 * fract(4.0 * hx2) - 4.5) * 0.03 + 0.025;
    float treesm = 1.0 - step(trees, uv.y);
    float treess = smoothstep(trees - animShadowLength, trees, uv.y);
    treess -= 1.0 - treesm;
    treess *= _shadowIntensity;

    //Place hill and tree layers on background with cast shadow
    final = mix(final, _hillsL1Color, hillsL1m);
    final = mix(final, _hillsL1Color - _shadowDarkness, hillsL1s);
    final = mix(final, _hillsL2Color, hillsL2m);
    final = mix(final, _hillsL2Color - _shadowDarkness, hillsL2s);
    final = mix(final, _treesColor, treesm);
    final = mix(final, _treesColor - _shadowDarkness, treess);

    //Output to screen
    FragColor = vec4(final, 1.0);
}