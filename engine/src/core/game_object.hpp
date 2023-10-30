#pragma once



namespace ts
{
struct Model final
{
    size_t firstIndex;
    size_t indexCount;
};

inline std::array<math::Vec3, 2> lightUniformData
{{
    {0.f, 5.f, -7.f},
    {0.f, 5.f,  0.f},
}};
}