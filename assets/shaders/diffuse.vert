#version 450

#extension GL_EXT_multiview : enable

layout(binding = 0) uniform World
{
    mat4 matrix;
} world;

layout(binding = 1) uniform Foo
{
    mat4 cameraMatrix;
    mat4 viewMatrices[2];
    mat4 projMatrices[2];
} foo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec3 color;

void main()
{
    gl_Position = foo.projMatrices[gl_ViewIndex] * foo.viewMatrices[gl_ViewIndex] * foo.cameraMatrix * world.matrix * vec4(inPosition, 1.0);

    normal = normalize(vec3(world.matrix * vec4(inNormal, 0.0)));
    color = inColor;
}