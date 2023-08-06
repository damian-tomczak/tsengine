#version 450

layout(location = 0) in vec3 inColor;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform LightUbo {
    vec3[2] lightPositions;
} lightUbo;

void main()
{
    outColor = vec4(inColor, 1.0);
}