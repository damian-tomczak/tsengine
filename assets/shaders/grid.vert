#version 450

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_multiview : enable
#extension GL_EXT_debug_printf : enable

#include "assets/shaders/grid_params.h"

layout (location=0) out vec2 uv;
layout (location=1) out vec2 cameraPosition;

layout(binding = 1) uniform Ubo{
    vec3 cameraPosition;
    mat4 viewMat[2];
    mat4 projMat[2];
} ubo;

void main()
{
    mat4 cameraMat = mat4(1.0);
    cameraMat[3] = vec4(ubo.cameraPosition, 1.0);

    mat4 MVP = ubo.projMat[gl_ViewIndex] * ubo.viewMat[gl_ViewIndex] * cameraMat;

    int idx = indices[gl_VertexIndex];
    vec3 position = pos[idx] * gridSize;

    mat4 iViewMat = inverse(ubo.viewMat[gl_ViewIndex] * cameraMat);
    cameraPosition = vec2(iViewMat[3][0], iViewMat[3][2]);

    position.x += cameraPosition.x;
    position.z += cameraPosition.y;

    uv = position.xz;
    gl_Position = MVP * vec4(position, 1.0);
}
