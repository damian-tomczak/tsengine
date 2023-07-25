#pragma once

// ACHTUNG --- ONLY FOR DEVELOPMENT PURPOSES --- ACHTUNG
// TODO: refactor it to the ecs

#include "tsengine/math.hpp"
#include "tiny_obj_loader.h"
#include "utils.hpp"

namespace ts
{
struct Model final
{
    size_t firstIndex;
    size_t indexCount;
    math::Matrix4x4<> worldMatrix;
};

struct Vertex final
{
    math::Vec3<> position;
    math::Vec3<> normal;
    math::Vec3<> color;
};

class MeshData final
{
    NOT_COPYABLE_AND_MOVEABLE(MeshData);

public:
    MeshData() = default;

    void loadModel(std::string_view filename, std::vector<Model*>& models, size_t count)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        if (!tinyobj::LoadObj(&attrib, &shapes, nullptr, nullptr, nullptr, filename.data()))
        {
            LOGGER_ERR(std::format("can not open the {} model", filename.data()).c_str());
        }

        const auto oldIndexCount{mIndices.size()};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{
                    .position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    },
                    .color = { 1.0f, 1.0f, 1.0f }
                };

                if (index.normal_index >= 0)
                {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                mVertices.push_back(vertex);
                mIndices.push_back(static_cast<uint32_t>(mIndices.size()));
            }
        }

        static size_t offset{0};
        for (size_t modelIndex{offset}; modelIndex < (offset + count); ++modelIndex)
        {
            auto pModel = models.at(modelIndex);
            pModel->firstIndex = oldIndexCount;
            pModel->indexCount = mIndices.size() - oldIndexCount;
        }
    }

private:
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;
};
}