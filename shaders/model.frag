#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 colorMod;
layout(location = 3) in vec4 worldPosition;
layout(location = 4) in vec4 cameraPos;
layout(location = 5) in vec4 lightPos;
layout(location = 6) in vec4 lightColor;

layout(location = 0) out vec4 outColor;


void main()
{
    vec3 lightDir = normalize(lightPos.xyz - worldPosition.xyz);
    vec4 texColor = texture(texSampler, fragTexCoord);

    texColor.xyz = texColor.xyz * max(0.0,dot(-lightDir,inNormal));

    outColor = texColor * colorMod;
}