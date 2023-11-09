#pragma once

#ifdef TESTER_ADAPTER

#include "tsengine/core.h"

#include <chrono>

namespace ts
{
struct TesterEngine : public Engine
{
    TesterEngine(std::chrono::steady_clock::duration renderingDuration_) : renderingDuration{renderingDuration_}
    {}

    TesterEngine(const TesterEngine&) = delete;
    TesterEngine& operator=(TesterEngine&) = delete;
    TesterEngine(const TesterEngine&&) = delete;
    TesterEngine& operator=(TesterEngine&&) = delete;

    virtual bool tick() override { return true; }
    virtual void loadLvL() override
    {}

    std::chrono::steady_clock::duration renderingDuration;
};
}

#endif