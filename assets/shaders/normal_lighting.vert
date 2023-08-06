#version 450

#extension GL_EXT_multiview : enable

layout(binding = 0) uniform IndividualUbo {
    mat4 modelMat;
} individualUbo;

layout(binding = 1) uniform CommonUbo {
    vec3 cameraPosition;
    mat4 viewMatrices[2];
    mat4 projectionMatrices[2];
} commonUbo;

layout(push_constant) uniform PushConsts{
    vec3 objPos;
} pushConsts;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outColor;

void main()
{
    mat4 cameraMat = mat4(1.0);
    cameraMat[3] = vec4(commonUbo.cameraPosition, 1.0);

    gl_Position =
        commonUbo.projectionMatrices[gl_ViewIndex] *
        commonUbo.viewMatrices[gl_ViewIndex] *
        cameraMat *
        individualUbo.modelMat *
        vec4(pushConsts.objPos + inPos, 1.0);

    outColor = mat3(individualUbo.modelMat) * normalize(inNormal);
}