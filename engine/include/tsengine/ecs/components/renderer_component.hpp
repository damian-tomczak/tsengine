#pragma once

#include "tsengine/math.hpp"

namespace ts
{
class Renderer;

// TODO: enum reflection
// TODO: implement color

#define TS_PIPELINES_LIST     \
    PIPELINE(COLOR)           \
    PIPELINE(NORMAL_LIGHTING) \
    PIPELINE(PBR)             \
    PIPELINE(LIGHT)           \
    PIPELINE(GRID)            \


enum class PipelineType
{
    INVALID,
#define PIPELINE(type) type, 
    TS_PIPELINES_LIST
#undef PIPELINE
    COUNT
};

struct RendererComponentBase : public Component
{
    using Base = RendererComponentBase;
    using ZIdxT = int32_t;

    RendererComponentBase(const ZIdxT z_ = {}) : z{z_} {}

    ZIdxT z;
};

template<PipelineType pipeType>
struct RendererComponent : public RendererComponentBase 
{
    PipelineType pipeline{pipeType};

    RendererComponent(const ZIdxT z_ = {}) : RendererComponentBase{z_}
    {}
};

template<>
struct RendererComponent<PipelineType::PBR> : public RendererComponentBase
{
#define TS_MATERIALS_LIST \
    MATERIAL(WHITE,    .color = {1.0f     , 1.0f      ,1.0f     }, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(RED,      .color = {1.0f     , 0.0f      ,0.0f     }, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(BLUE,     .color = {0.0f     , 0.0f      ,1.0f     }, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(BLACK,    .color = {0.0f     , 0.0f      ,0.0f     }, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(GOLD,     .color = {1.0f     , 0.765557f, 0.336057f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(COPPER,   .color = {0.955008f, 0.637427f, 0.538163f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(CHROMIUM, .color = {0.549585f, 0.556114f, 0.554256f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(NICKEL,   .color = {0.659777f, 0.608679f, 0.525649f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(TITANIUM, .color = {0.541931f, 0.496791f, 0.449419f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(COBALT,   .color = {0.662124f, 0.654864f, 0.633732f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(PLATINUM, .color = {0.672411f, 0.637331f, 0.585456f}, .roughness = 0.5f, .metallic = 1.0f) \

    struct Material
    {
        enum class Type
        {
            INVALID,
#define MATERIAL(type, ...) type, 
            TS_MATERIALS_LIST
#undef MATERIAL
            COUNT
        };

        static constexpr Material create(const Material::Type type)
        {
            switch (type)
            {
#define MATERIAL(type, ...)          \
    case Material::Type::type:       \
        return Material{__VA_ARGS__};

                TS_MATERIALS_LIST
#undef MATERIAL
            default: throw Exception{"Invalid material type"};
            }

            return {};
        }

        math::Vec3 color;
        float roughness;
        float metallic;
    };

    Material material;

    RendererComponent(const Material material_ = Material::create(Material::Type::RED), const ZIdxT z_ = {})
        : RendererComponentBase{z_}, material{material_}
    {}
};
}
