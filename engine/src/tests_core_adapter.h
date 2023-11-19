#pragma once

#ifdef TESTER_ADAPTER

#include "tsengine/core.h"

#include <chrono>

namespace ts
{
inline namespace TS_VER
{
struct TesterEngine : public Engine
{
    TesterEngine(std::chrono::steady_clock::duration renderingDuration_) : renderingDuration{renderingDuration_}
    {}

    TesterEngine(const TesterEngine&) = delete;
    TesterEngine& operator=(TesterEngine&) = delete;
    TesterEngine(const TesterEngine&&) = delete;
    TesterEngine& operator=(TesterEngine&&) = delete;

    virtual bool tick(const float dt) override { return true; }
    virtual void loadLvL() override
    {}

    std::chrono::steady_clock::duration renderingDuration;
};
} // namespace ver
} // namespace ts

#endif