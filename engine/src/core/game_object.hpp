#pragma once

// TODO: refactor it to the ecs

#include "internal_utils.h"
#include "tsengine/math.hpp"
#include "tsengine/logger.h"

#include "tiny_obj_loader.h"

namespace ts
{
// TODO: enum reflection
enum class PipelineType
{
    INVALID,
    COLOR, // TODO: implement
    NORMAL_LIGHTING,
    PBR,
    COUNT
};

#define TS_MATERIALS_LIST \
    MATERIAL(WHITE,    .color = {1.0f, 1.0f, 1.0f},                .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(RED,      .color = {1.0f, 0.0f, 0.0f},                .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(BLUE,     .color = {0.0f, 0.0f, 1.0f},                .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(BLACK,    .color = {0.0f, 0.0f, 0.0f},                .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(GOLD,     .color = {1.0f, 0.765557f, 0.336057f},      .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(COPPER,   .color = {0.955008f, 0.637427f, 0.538163f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(CHROMIUM, .color = {0.549585f, 0.556114f, 0.554256f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(NICKEL,   .color = {0.659777f, 0.608679f, 0.525649f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(TITANIUM, .color = {0.541931f, 0.496791f, 0.449419f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(COBALT,   .color = {0.662124f, 0.654864f, 0.633732f}, .roughness = 0.5f, .metallic = 1.0f) \
    MATERIAL(PLATINUM, .color = {0.672411f, 0.637331f, 0.585456f}, .roughness = 0.5f, .metallic = 1.0f) \

struct Material final
{
    enum class Type
    {
#define MATERIAL(type, ...) type, 
        TS_MATERIALS_LIST
#undef MATERIAL
    };


    math::Vec3 color;
    float roughness;
    float metallic;
};

struct Model final
{
    size_t firstIndex;
    size_t indexCount;

    math::Vec3 pos;
    math::Mat4 model;
    PipelineType pipeline;
    Material material;
};

struct Vertex final
{
    math::Vec3 position;
    math::Vec3 normal;
    math::Vec3 color;
};

class MeshData final
{
    NOT_COPYABLE_AND_MOVEABLE(MeshData);

public:
    MeshData() = default;

    void loadModel(const std::string& fileName, const std::vector<std::shared_ptr<Model>>& models, size_t count)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        if (!tinyobj::LoadObj(&attrib, &shapes, nullptr, nullptr, nullptr, fileName.data()))
        {
            LOGGER_ERR(("Can not open the file: " + fileName).c_str());
        }

        const auto oldIndexCount = mIndices.size();

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{
                    .position =
                    {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    },
                };

                if (index.normal_index >= 0)
                {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                vertex.color = vertex.normal;

                mVertices.emplace_back(std::move(vertex));
                mIndices.emplace_back(static_cast<uint32_t>(mIndices.size()));
            }
        }

        for (size_t modelIndex{offset}; modelIndex < (offset + count); ++modelIndex)
        {
            auto model = models.at(modelIndex);
            model->firstIndex = oldIndexCount;
            model->indexCount = mIndices.size() - oldIndexCount;
        }
        offset += count;
    }

    size_t getIndexOffset() const { return sizeof(mVertices.at(0)) * mVertices.size(); }
    size_t getSize() const { return sizeof(mVertices.at(0)) * mVertices.size() + sizeof(mIndices.at(0)) * mIndices.size(); }
    void writeTo(char* destination) const
    {
        const size_t verticesSize = sizeof(mVertices.at(0)) * mVertices.size();
        const size_t indicesSize = sizeof(mIndices.at(0)) * mIndices.size();
        memcpy(destination, mVertices.data(), verticesSize);
        memcpy(destination + verticesSize, mIndices.data(), indicesSize);
    }

private:
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;

    size_t offset{};
};

struct Materials final
{
    static consteval Material create(const Material::Type type)
    {
        switch (type)
        {
#define MATERIAL(type, ...) \
    case Material::Type::type: \
        return Material{__VA_ARGS__};
            TS_MATERIALS_LIST
#undef MATERIAL
        }
        return{};
    }
};

inline std::array<math::Vec3, 2> lightUniformData
{{
    {0.f, 5.f, -5.f},
    {0.f, 5.f,  5.f},
}};
}