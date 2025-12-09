#version 310 es
precision highp float;

layout(binding = 1) uniform sampler2D skyTex;

layout(location = 0) in vec3 vDir;
layout(location = 1) in vec4 colorMod;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 d = normalize(vDir);

    // Direction → spherical UV
    float u = atan(d.z, d.x) / (2.0 * 3.14159265) + 0.5;
    float v = acos(d.y) / 3.14159265;

    outColor = texture(skyTex, vec2(u, v)) * colorMod;
}