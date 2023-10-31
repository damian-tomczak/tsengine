#version 450

#extension GL_EXT_multiview : enable
#extension GL_EXT_debug_printf : enable

layout(binding = 1) uniform Ubo {
    vec3 camPos;
    mat4 viewMat[2];
    mat4 projMats[2];
} ubo;

layout(push_constant) uniform PushConst {
    vec3 objPos;
} pushConst;

const vec3 cubeVertices[36] =
{
    {-1., -1., -1.},
    {-1., -1.,  1.},
    {-1.,  1.,  1.},
    { 1.,  1., -1.},
    {-1., -1., -1.},
    {-1.,  1., -1.},
    { 1., -1.,  1.},
    {-1., -1., -1.},
    { 1., -1., -1.},
    { 1.,  1., -1.},
    { 1., -1., -1.},
    {-1., -1., -1.},
    {-1., -1., -1.},
    {-1.,  1.,  1.},
    {-1.,  1., -1.},
    { 1., -1.,  1.},
    {-1., -1.,  1.},
    {-1., -1., -1.},
    {-1.,  1.,  1.},
    {-1., -1.,  1.},
    { 1., -1.,  1.},
    { 1.,  1.,  1.},
    { 1., -1., -1.},
    { 1.,  1., -1.},
    { 1., -1., -1.},
    { 1.,  1.,  1.},
    { 1., -1.,  1.},
    { 1.,  1.,  1.},
    { 1.,  1., -1.},
    {-1.,  1., -1.},
    { 1.,  1.,  1.},
    {-1.,  1., -1.},
    {-1.,  1.,  1.},
    { 1.,  1.,  1.},
    {-1.,  1.,  1.},
    { 1., -1.,  1.},
};

void main()
{
    mat4 cameraMat = mat4(1.0);
    cameraMat[3] = vec4(ubo.camPos, 1.0);

    gl_Position =
        ubo.projMats[gl_ViewIndex] *
        ubo.viewMat[gl_ViewIndex] *
        cameraMat *
        vec4(pushConst.objPos + cubeVertices[gl_VertexIndex], 1.0);
}