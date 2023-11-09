#include "tsengine/asset_store.h"

#include "globals.hpp"
#include "tsengine/logger.h"

#include "tsengine/ecs/ecs.h"
#include "tsengine/ecs/components/mesh_component.hpp"
#include "tsengine/ecs/components/transform_component.hpp"

#include "tiny_obj_loader.h"

namespace ts
{
namespace
{
    std::vector<MeshComponent::Vertex> mVertices;
    std::vector<uint32_t> mIndices;
}

// TODO: avoid the same files
void AssetStore::Models::load()
{
    for (const auto entity : gReg.getSystem<AssetStore>().getSystemEntities())
    {
        auto& meshComponent = entity.getComponent<MeshComponent>();
        const auto& fileName = meshComponent.assetName;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        if (!tinyobj::LoadObj(&attrib, &shapes, nullptr, nullptr, nullptr, fileName.data()))
        {
            TS_ERR(("Can not open the file: " + fileName).c_str());
        }

        const auto oldIndexCount = mIndices.size();

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                MeshComponent::Vertex vertex{
                    .position = {
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

        meshComponent.firstIndex = oldIndexCount;
        meshComponent.indexCount = mIndices.size() - oldIndexCount;
    }
}

void AssetStore::Models::writeTo(char* const destination)
{
    const size_t verticesSize = sizeof(mVertices.at(0)) * mVertices.size();
    const size_t indicesSize = sizeof(mIndices.at(0)) * mIndices.size();
    memcpy(destination, mVertices.data(), verticesSize);
    memcpy(destination + verticesSize, mIndices.data(), indicesSize);
}

size_t AssetStore::Models::getIndexOffset()
{
    return sizeof(mVertices.at(0)) * mVertices.size();
}

size_t AssetStore::Models::getSize()
{
    return sizeof(mVertices.at(0)) * mVertices.size() + sizeof(mIndices.at(0)) * mIndices.size();
}
}