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

struct Material final
{
    std::string_view name;

    struct Params final
    {
        math::Vec3 color;
        float roughness;
        float metallic;
    } params;
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
    // TODO: rewrite it for enums
    static constexpr std::array materials
    {
        Material{.name = "White"   , .params{.color = {1.0f},                            .roughness = 0.5f, .metallic = 1.0f}},
        Material{.name = "Red"     , .params{.color = {1.0f, 0.0f, 0.0f},                .roughness = 0.5f, .metallic = 1.0f}},
        Material{.name = "Blue"    , .params{.color = {0.0f, 0.0f, 1.0f},                .roughness = 0.5f, .metallic = 1.0f}},
        Material{.name = "Black"   , .params{.color = {0.0f},                            .roughness = 0.5f, .metallic = 1.0f}},
        Material{.name = "Gold"    , .params{.color = {1.0f, 0.765557f, 0.336057f},      .roughness = 0.5f, .metallic = 1.0f}},
        Material{.name = "Copper"  , .params{.color = {0.955008f, 0.637427f, 0.538163f}, .roughness = 0.5f, .metallic = 1.0f}},
        Material{.name = "Chromium", .params{.color = {0.549585f, 0.556114f, 0.554256f}, .roughness = 0.5f, .metallic = 1.0f}},
        Material{.name = "Nickel"  , .params{.color = {0.659777f, 0.608679f, 0.525649f}, .roughness = 0.5f, .metallic = 1.0f}},
        Material{.name = "Titanium", .params{.color = {0.541931f, 0.496791f, 0.449419f}, .roughness = 0.5f, .metallic = 1.0f}},
        Material{.name = "Cobalt"  , .params{.color = {0.662124f, 0.654864f, 0.633732f}, .roughness = 0.5f, .metallic = 1.0f}},
        Material{.name = "Platinum", .params{.color = {0.672411f, 0.637331f, 0.585456f}, .roughness = 0.5f, .metallic = 1.0f}},
    };

    // TODO: constexpr
    static Material at(const std::string& materialName)
    {
        const auto it = std::ranges::find_if(materials, [&materialName](const auto& material) -> bool {
            return materialName == material.name;
        });

        if (it == materials.end())
        {
            LOGGER_ERR(std::format(R"(Material "{}" doesn't exist)", materialName).c_str());
        }

        return *it;
    }
};
}