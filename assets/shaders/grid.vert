#version 460

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_multiview : enable

#include "assets/shaders/grid_params.h"

layout (location=0) out vec2 uv;
layout (location=1) out vec2 cameraPos;

layout(binding = 1) uniform Ubo
{
    mat4 camera;
    mat4 view[2];
    mat4 proj[2];
} ubo;

void main()
{
    mat4 MVP = ubo.proj[gl_ViewIndex] * ubo.view[gl_ViewIndex];

	int idx = indices[gl_VertexIndex];
	vec3 position = pos[idx] * gridSize;

	mat4 iview = inverse(ubo.view[gl_ViewIndex]);
	cameraPos = vec2(iview[3][0], iview[3][2]);

	position.x += cameraPos.x;
	position.z += cameraPos.y;

	gl_Position = MVP * vec4(position, 1.0);
	uv = position.xz;
}
