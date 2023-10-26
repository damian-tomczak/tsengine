#version 450

#extension GL_EXT_multiview : enable
#extension GL_EXT_debug_printf : enable

layout(binding = 0) uniform IndividualUbo {
    mat4 modelMat;
} individualUbo;

layout (binding = 1) uniform CommonUbo {
    vec3 camPos;
    mat4 viewMats[2];
    mat4 projMats[2];
} commonUbo;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;

layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec3 outNormal;

layout(push_constant) uniform PushConst {
    vec3 objPos;
} pushConst;

void main()
{
    vec3 worldPos = inPos + pushConst.objPos;
    outWorldPos = worldPos;
    outNormal = mat3(individualUbo.modelMat) * inNormal;

    gl_Position =
        commonUbo.projMats[gl_ViewIndex] *
        commonUbo.viewMats[gl_ViewIndex] *
        individualUbo.modelMat *
        vec4(worldPos, 1.0);

}
