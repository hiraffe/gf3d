#version 310 es
precision highp float;

layout(binding = 0) uniform SkyUBO {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 color;
} ubo;

layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec3 inNormal;
//layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 vDir;
layout(location = 1) out vec4 colorMod;

void main()
{
    // Remove camera translation
    mat3 rotView = mat3(ubo.view);

    // Direction vector for sampling
    vDir = rotView * inPosition;

    // Position skybox: depth = 1.0
    vec4 pos = ubo.proj * vec4(inPosition, 1.0);
    pos.z = pos.w;
    gl_Position = pos;

    colorMod = ubo.color;
}