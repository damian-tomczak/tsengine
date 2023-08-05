#include "controllers.h"
#include "khronos_utils.h"

namespace
{
XrPath stringToXrPath(XrInstance instance, std::string_view str)
{
    XrPath path;
    LOGGER_XR(xrStringToPath, instance, str.data(), &path);

    return path;
}
}

namespace ts
{
Controllers::~Controllers()
{
    for (auto& space : mSpaces)
    {
        xrDestroySpace(space);
    }

    if (mFlyAction != nullptr)
    {
        xrDestroyAction(mFlyAction);
    }

    if (mPoseAction != nullptr)
    {
        xrDestroyAction(mPoseAction);
    }

    if (mActionSet != nullptr)
    {
        xrDestroyActionSet(mActionSet);
    }
}

void Controllers::setupControllers()
{
    XrActionSetCreateInfo actionSetCreateInfo{XR_TYPE_ACTION_SET_CREATE_INFO};

    strcpy(actionSetCreateInfo.actionSetName, actionSetName.data());
    strcpy(actionSetCreateInfo.localizedActionSetName, localizedActionSetName.data());

    LOGGER_XR(xrCreateActionSet, mInstance, &actionSetCreateInfo, &mActionSet);

    mPaths.at(0) = stringToXrPath(mInstance, "/user/hand/left");
    mPaths.at(1) = stringToXrPath(mInstance, "/user/hand/right");

    createAction("handpose", "Hand Pose", XR_ACTION_TYPE_POSE_INPUT, mPoseAction);
    createAction("fly", "Fly", XR_ACTION_TYPE_FLOAT_INPUT, mFlyAction);
    createAction("trigget_action", "Trigger Action", XR_ACTION_TYPE_BOOLEAN_INPUT, mTriggerAction);

    for (size_t controllerIndex{}; controllerIndex < controllerCount; ++controllerIndex)
    {
        XrActionSpaceCreateInfo actionSpaceCreateInfo{
            .type = XR_TYPE_ACTION_SPACE_CREATE_INFO,
            .action = mPoseAction,
            .subactionPath = mPaths.at(controllerIndex),
            .poseInActionSpace = khronos_utils::makeXrIdentity()
        };

        LOGGER_XR(xrCreateActionSpace, mSession, &actionSpaceCreateInfo, &mSpaces.at(controllerIndex));
    }

    const std::array bindings{
        XrActionSuggestedBinding{mPoseAction, stringToXrPath(mInstance, "/user/hand/left/input/aim/pose")     },
        XrActionSuggestedBinding{mPoseAction, stringToXrPath(mInstance, "/user/hand/right/input/aim/pose")    },
        XrActionSuggestedBinding{mFlyAction , stringToXrPath(mInstance, "/user/hand/left/input/select/click") },
        XrActionSuggestedBinding{mFlyAction , stringToXrPath(mInstance, "/user/hand/right/input/select/click")},
    };

    XrInteractionProfileSuggestedBinding interactionProfileSuggestedBinding{
        .type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING,
        .interactionProfile = stringToXrPath(mInstance, interactionProfile),
        .countSuggestedBindings = static_cast<uint32_t>(bindings.size()),
        .suggestedBindings = bindings.data(),
    };

    LOGGER_XR(xrSuggestInteractionProfileBindings, mInstance, &interactionProfileSuggestedBinding);

    XrSessionActionSetsAttachInfo sessionActionSetsAttachInfo{
        .type = XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO,
        .countActionSets = 1,
        .actionSets = &mActionSet
    };

    LOGGER_XR(xrAttachSessionActionSets, mSession, &sessionActionSetsAttachInfo);
}

bool Controllers::sync(const XrSpace space, const XrTime time)
{
    const XrActiveActionSet activeActionSet{
        .actionSet = mActionSet
    };

    XrActionsSyncInfo actionsSyncInfo{
        .type = XR_TYPE_ACTIONS_SYNC_INFO,
        .countActiveActionSets = 1,
        .activeActionSets = &activeActionSet
    };

    LOGGER_XR(xrSyncActions, mSession, &actionsSyncInfo);

    for (size_t controllerIndex{}; controllerIndex < controllerCount; ++controllerIndex)
    {
        const auto& path = mPaths.at(controllerIndex);

        XrActionStatePose poseState{XR_TYPE_ACTION_STATE_POSE};
        updateActionStatePose(mSession, mPoseAction, path, poseState);

        if (poseState.isActive)
        {
            XrSpaceLocation spaceLocation{ XR_TYPE_SPACE_LOCATION };
            LOGGER_XR(xrLocateSpace, mSpaces.at(controllerIndex), space, time, &spaceLocation);

            constexpr XrSpaceLocationFlags checkFlags{
                XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_POSITION_TRACKED_BIT |
                XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT};

            if ((spaceLocation.locationFlags & checkFlags) == checkFlags)
            {
                mPoses.at(controllerIndex) = khronos_utils::xrPoseToMatrix(spaceLocation.pose);
            }
        }

        XrActionStateFloat flySpeedState{XR_TYPE_ACTION_STATE_FLOAT};
        updateActionStateFloat(mSession, mFlyAction, path, flySpeedState);

        if (flySpeedState.isActive)
        {
            mFlySpeeds.at(controllerIndex) = flySpeedState.currentState;
        }
    }

    return true;
}

void Controllers::updateActionStatePose(const XrSession session, const XrAction action, const XrPath path, XrActionStatePose& state)
{
    XrActionStateGetInfo actionStateGetInfo{
        .type = XR_TYPE_ACTION_STATE_GET_INFO,
        .action = action,
        .subactionPath = path
    };

    LOGGER_XR(xrGetActionStatePose, session, &actionStateGetInfo, &state);
}

void Controllers::updateActionStateFloat(const XrSession session, const XrAction action, const XrPath path, XrActionStateFloat& state)
{
    XrActionStateGetInfo actionStateGetInfo{
        .type = XR_TYPE_ACTION_STATE_GET_INFO,
        .action = action,
        .subactionPath = path
    };

    LOGGER_XR(xrGetActionStateFloat, session, &actionStateGetInfo, &state);
}

void Controllers::createAction(const std::string& actionName, const std::string& localizedActionName, const XrActionType type, XrAction& action)
{
    const XrActionCreateInfo actionCreateInfo{
        .type = XR_TYPE_ACTION_CREATE_INFO,
        .actionType = type,
        .countSubactionPaths = static_cast<uint32_t>(mPaths.size()),
        .subactionPaths = mPaths.data()
    };

    strcpy(const_cast<char*>(actionCreateInfo.actionName), actionName.data());
    strcpy(const_cast<char*>(actionCreateInfo.localizedActionName), localizedActionName.data());

    LOGGER_XR(xrCreateAction, mActionSet, &actionCreateInfo, &action);
}
}