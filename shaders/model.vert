#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
    mat4    model;
    mat4    view;
    mat4    proj;
    vec4    color;
    vec4    camera;
    vec4    lightPos;
    vec4    lightColor;
} ubo;

layout(set = 0, binding = 1) uniform Bones {
    mat4 boneMatrices[64];
} bones;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in uvec4 inJoint; // or ivec4 depending on upload
layout(location = 4) in vec4 inWeight;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 colorMod;
layout(location = 3) out vec4 worldPosition;
layout(location = 4) out vec4 cameraPos;
layout(location = 5) out vec4 lightPos;
layout(location = 6) out vec4 lightColor;

void main()
{
    mat3 normalMatrix;
    mat4 mvp = ubo.proj * ubo.view * ubo.model;

    //positions
    gl_Position =  mvp * vec4(inPosition, 1.0);
    worldPosition = ubo.model * vec4(inPosition,1.0);

    //normals
    normalMatrix = transpose(inverse(mat3(ubo.model)));
    outNormal = normalize(normalMatrix*inNormal);

    /*
    vec4 skPos = vec4(0.0);
    vec3 skNorm = vec3(0.0);

    for (int i = 0; i < 4; ++i) {
        uint j = inJoint[i];
        float w = inWeight[i];
        if (w <= 0.0) continue;
        mat4 bm = bones.boneMatrices[j];
        skPos += bm * vec4(inPosition, 1.0) * w;
        skNorm += mat3(bm) * inNormal * w;
    }

    vec4 worldPos = ubo.model * skPos;
    gl_Position = ubo.proj * ubo.view * worldPos;
    */

    //pass throughs
    colorMod = ubo.color;
    cameraPos = ubo.camera;
    fragTexCoord = inTexCoord;
    lightPos = ubo.lightPos;
    lightPos.x+=100;
    lightColor = ubo.lightColor;
}