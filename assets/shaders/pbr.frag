#version 450

#extension GL_EXT_multiview : enable
#extension GL_EXT_debug_printf : enable

#define PI 3.14159265359

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform CommonUbo {
    vec3 camPos;
    mat4 viewMat[2];
    mat4 projMat[2];
} commonUbo;

layout (binding = 2) uniform LightsUbo {
    vec3 lights[2];
} lightsUbo;

layout(push_constant) uniform Material {
    layout(offset = 16) float r;
    layout(offset = 20) float g;
    layout(offset = 24) float b;
    layout(offset = 32) float roughness;
    layout(offset = 36) float metallic;
} material;

vec3 materialcolor()
{
    return vec3(material.r, material.g, material.b);
}

float D_GGX(float dotNH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2)/(PI * denom*denom);
}

float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float GL = dotNL / (dotNL * (1.0 - k) + k);
    float GV = dotNV / (dotNV * (1.0 - k) + k);
    return GL * GV;
}

vec3 F_Schlick(float cosTheta, float metallic)
{
    vec3 F0 = mix(vec3(0.04), materialcolor(), metallic);
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    return F;
}

vec3 BRDF(vec3 L, vec3 V, vec3 N, float metallic, float roughness)
{
    vec3 H = normalize (V + L);
    float dotNV = clamp(dot(N, V), 1e-5, 1.0);
    float dotNL = clamp(dot(N, L), 1e-5, 1.0);
    float dotLH = clamp(dot(L, H), 1e-5, 1.0);
    float dotNH = clamp(dot(N, H), 1e-5, 1.0);

    vec3 lightColor = vec3(1.0);

    vec3 color = vec3(0.0);

    if (dotNL > 0.0)
    {
        float rroughness = max(0.05, roughness);
        float D = D_GGX(dotNH, roughness);
        float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness);
        vec3 F = F_Schlick(dotNV, metallic);

        vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

        color += spec * dotNL * lightColor;
    }

    return color;
}

void main()
{
    vec3 N = normalize(inNormal);
    vec3 V = normalize(commonUbo.camPos - inWorldPos);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < lightsUbo.lights.length(); i++)
    {
        vec3 L = normalize(lightsUbo.lights[i].xyz - inWorldPos);
        Lo += BRDF(L, V, N, material.metallic, material.roughness);
    };

    vec3 color = materialcolor() * 0.02;
    color += Lo;

    color = pow(color, vec3(0.4545));

    outColor = vec4(color, 1.0);
}