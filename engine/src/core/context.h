#pragma once

#include "pch.h"
#include "tools.h"

#include "vulkan/shaders_compiler.h"
#include "vulkan/vulkan_loader.h"
#include "openxr/openxr_platform.h"
#include "os.h"

namespace ts
{
class Context final : public Singleton<Context>
{
    SINGLETON_BODY(Context);
    NOT_COPYABLE_AND_MOVEABLE(Context);

public:
    void createContext(const std::string_view& appName);

private:
    void createOpenXRInstance();

    std::string_view mAppName;
    XrInstance mXrInstance{};
};
}