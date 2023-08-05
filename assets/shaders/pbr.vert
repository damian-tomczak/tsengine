#version 450

#extension GL_EXT_multiview : enable

layout(binding = 0) uniform Ubo{
    mat4 cameraMat;
    mat4 viewMats[2];
    mat4 projMats[2];
} ubo;

layout(push_constant) uniform PushConsts{
    vec3 objPos;
} pushConsts;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 outColor;

void main()
{
    gl_Position =
        ubo.projMats[gl_ViewIndex] *
        ubo.viewMats[gl_ViewIndex] *
        ubo.cameraMat *
        vec4(pushConsts.objPos + inPosition, 1.0);

    outColor = normalize(inNormal);
}