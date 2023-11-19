#pragma once

#include "tsengine/ecs/ecs.h"
#include "tsengine/math.hpp"

#include <string>

struct ExampleComponent : public ts::Component
{
    ts::math::Vec3 startPos;
    ts::math::Vec3 endPos;
    std::chrono::high_resolution_clock::time_point startTime;

    ExampleComponent(ts::math::Vec3 startPos_ = {}, ts::math::Vec3 endPos_ = {}) :
        startPos{startPos_},
        endPos{endPos_},
        startTime{std::chrono::high_resolution_clock::now()}
    {}
};