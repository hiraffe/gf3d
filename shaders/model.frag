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
    // normalize inputs
    vec3 norm = normalize(inNormal);
    vec3 lightDir = normalize(lightPos.xyz - worldPosition.xyz);
    vec3 viewDir = normalize(cameraPos.xyz - worldPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);

    // texture color
    vec3 texColor = texture(texSampler, fragTexCoord).rgb;

    // --- lighting components ---
    float diff = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // shininess = 32

    vec3 ambient = 0.25 * lightColor.rgb;     // soft global light
    vec3 diffuse = diff * lightColor.rgb;     // main light brightness
    vec3 specular = 0.5 * spec * lightColor.rgb; // highlight reflection

    // combine
    vec3 finalColor = (ambient + diffuse + specular) * texColor * colorMod.rgb;

    outColor = vec4(finalColor, colorMod.a);
}