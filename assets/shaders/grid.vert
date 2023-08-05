#version 450

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_multiview : enable

#include "assets/shaders/grid_params.h"

layout (location=0) out vec2 uv;
layout (location=1) out vec2 cameraPos;

layout(binding = 0) uniform Ubo{
    mat4 cameraMat;
    mat4 viewMat[2];
    mat4 projMat[2];
} ubo;

void main()
{
    mat4 MVP = ubo.projMat[gl_ViewIndex] * ubo.viewMat[gl_ViewIndex] * ubo.cameraMat;

    int idx = indices[gl_VertexIndex];
    vec3 position = pos[idx] * gridSize;

    mat4 iViewMat = inverse(ubo.viewMat[gl_ViewIndex]);
    cameraPos = vec2(iViewMat[3][0], iViewMat[3][2]);

    position.x += cameraPos.x;
    position.z += cameraPos.y;

    gl_Position = MVP * vec4(position, 1.0);
    uv = position.xz;
}
