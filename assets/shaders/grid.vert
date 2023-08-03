#version 460

#extension GL_GOOGLE_include_directive : enable

#include "assets/shaders/grid_params.h"

layout (location=0) out vec2 uv;
layout (location=1) out vec2 cameraPos;

layout(binding = 0) uniform World
{
    mat4 matrix;
} world;

layout(binding = 1) uniform ViewProjection
{
    mat4 matrices[2];
} viewProjection;

void main()
{
//	mat4 MVP = ubo.proj * ubo.view * ubo.model;
//
//	int idx = indices[gl_VertexIndex];
//	vec3 position = pos[idx] * gridSize;
//
//	mat4 iview = inverse(ubo.view);
//	cameraPos = vec2(iview[3][0], iview[3][2]);
//
//	position.x += cameraPos.x;
//	position.z += cameraPos.y;
//
//	gl_Position = MVP * vec4(position, 1.0);
//	uv = position.xz;
}
