#pragma once

// ACHTUNG --- ONLY FOR DEVELOPMENT PURPOSES --- ACHTUNG
// TODO: refactor it to the ecs

#include "tsengine/math.hpp"
#include "tiny_obj_loader.h"
#include "utils.hpp"
#include "tsengine/logger.h"

namespace ts
{
struct Model final
{
    size_t firstIndex;
    size_t indexCount;
    math::Mat4 worldMatrix;
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

    void loadModel(const std::string& fileName, std::vector<Model*>& models, size_t count)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        if (!tinyobj::LoadObj(&attrib, &shapes, nullptr, nullptr, nullptr, fileName.data()))
        {
            LOGGER_ERR(("can not open the model: " + fileName).c_str());
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
                    .color = { 1.f, 1.f, 1.f }
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
            auto model = models.at(modelIndex);
            model->firstIndex = oldIndexCount;
            model->indexCount = mIndices.size() - oldIndexCount;
        }
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
};
}