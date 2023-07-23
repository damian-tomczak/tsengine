#pragma once

#include "utils.hpp"
#include "openxr/openxr.h"
#include "tsengine/logger.h"
#include "tsengine/math.hpp"

namespace ts
{
class Controllers final
{
    NOT_COPYABLE_AND_MOVEABLE(Controllers);

    static constexpr size_t controllerCount = 2;
    static constexpr std::string_view actionSetName = "actionset";
    static constexpr std::string_view localizedActionSetName = "Actions";

public:
    Controllers(XrInstance xrInstance, XrSession xrSession) : mInstance(xrInstance), mSession(xrSession)
    {}
    ~Controllers();

    void setupControllers();

private:
    XrInstance mInstance{};
    XrSession mSession{};
    std::array<XrSpace, controllerCount> mSpaces{};
    XrActionSet mActionSet{};
    XrAction mPoseAction{}, mFlyAction{};
    std::array<XrPath, controllerCount> mPaths;
    std::array<Matrix4x4, controllerCount> mPoses;
    std::array<float, controllerCount> mFlySpeeds;

    void createAction(std::string_view actionName, std::string_view localizedActionName, XrActionType type, XrAction& action);
};
}
