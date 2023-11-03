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

    virtual bool init(const char*& gameName, unsigned& width, unsigned& height) override { return true; }

    virtual void close() override
    {}

    virtual bool tick() override { return true; }

    std::chrono::steady_clock::duration renderingDuration;
};
}

#endif