#pragma once

#include "utils.hpp"
#include "openxr/openxr.h"
#include "tsengine/math.hpp"

namespace ts
{
class Controllers final
{
    NOT_COPYABLE_AND_MOVEABLE(Controllers);

    static constexpr size_t controllerCount{2};
    static constexpr std::string_view actionSetName{"actionset"};
    static constexpr std::string_view localizedActionSetName{"Actions"};

public:
    Controllers(XrInstance xrInstance, XrSession xrSession) : mInstance(xrInstance), mSession(xrSession)
    {}
    ~Controllers();

    void setupControllers();
    bool sync(const XrSpace space, const XrTime time);

    float getFlySpeed(size_t controllerIndex) const { return mFlySpeeds.at(controllerIndex); }
    math::Mat4 getPose(size_t controllerIndex) const { return mPoses.at(controllerIndex); }

private:
    XrInstance mInstance{};
    XrSession mSession{};
    std::array<XrSpace, controllerCount> mSpaces{};
    XrActionSet mActionSet{};
    XrAction mPoseAction{}, mFlyAction{};
    std::array<XrPath, controllerCount> mPaths;
    std::array<math::Mat4, controllerCount> mPoses;
    std::array<float, controllerCount> mFlySpeeds;

    void createAction(
        const std::string& actionName,
        const std::string& localizedActionName,
        const XrActionType type,
        XrAction& action);

    void updateActionStatePose(const XrSession session, const XrAction action, const XrPath path, XrActionStatePose& state);
    void updateActionStateFloat(const XrSession session, const XrAction action, const XrPath path, XrActionStateFloat& state);
};
}
