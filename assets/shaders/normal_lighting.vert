#version 450

#extension GL_EXT_multiview : enable
#extension GL_EXT_debug_printf : enable

layout(binding = 0) uniform IndividualUbo {
    mat4 modelMat;
} individualUbo;

layout(binding = 1) uniform CommonUbo {
    vec3 camPos;
    mat4 viewMats[2];
    mat4 projMats[2];
} commonUbo;

layout(push_constant) uniform PushConst {
    vec3 objPos;
} pushConst;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outColor;

void main()
{
    mat4 cameraMat = mat4(1.0);
    cameraMat[3] = vec4(commonUbo.camPos, 1.0);

    gl_Position =
        commonUbo.projMats[gl_ViewIndex] *
        commonUbo.viewMats[gl_ViewIndex] *
        cameraMat *
        individualUbo.modelMat *
        vec4(pushConst.objPos + inPos, 1.0);

    outColor = mat3(individualUbo.modelMat) * normalize(inNormal);
}