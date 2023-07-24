#include "controllers.h"
#include "khronos_utils.hpp"
#include "tsengine/logger.h"

namespace
{
XrPath str2xrPath(XrInstance instance, std::string_view str)
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

    if (mFlyAction)
    {
        xrDestroyAction(mFlyAction);
    }

    if (mPoseAction)
    {
        xrDestroyAction(mPoseAction);
    }

    if (mActionSet)
    {
        xrDestroyActionSet(mActionSet);
    }
}

void Controllers::setupControllers()
{
    XrActionSetCreateInfo actionSetCreateInfo{ XR_TYPE_ACTION_SET_CREATE_INFO };

    memcpy(actionSetCreateInfo.actionSetName, actionSetName.data(), actionSetName.length() + 1);
    memcpy(actionSetCreateInfo.localizedActionSetName, localizedActionSetName.data(), localizedActionSetName.length() + 1);

    LOGGER_XR(xrCreateActionSet, mInstance, &actionSetCreateInfo, &mActionSet);

    mPaths.at(0) = str2xrPath(mInstance, "/user/hand/left");
    mPaths.at(1) = str2xrPath(mInstance, "/user/hand/right");

    createAction("handpose", "Hand Pose", XR_ACTION_TYPE_POSE_INPUT, mPoseAction);
    createAction("fly", "Fly", XR_ACTION_TYPE_FLOAT_INPUT, mFlyAction);

    for (size_t controllerIndex = 0; controllerIndex < controllerCount; ++controllerIndex)
    {
        XrActionSpaceCreateInfo actionSpaceCreateInfo{
            .type = XR_TYPE_ACTION_SPACE_CREATE_INFO,
            .action = mPoseAction,
            .subactionPath = mPaths.at(controllerIndex),
            .poseInActionSpace = khronos_utils::makeXrIdentity()
        };

        LOGGER_XR(xrCreateActionSpace, mSession, &actionSpaceCreateInfo, &mSpaces.at(controllerIndex));
    }

    // Suggest simple controller binding (generic)
    const std::array<XrActionSuggestedBinding, 4u> bindings{{
        { mPoseAction, str2xrPath(mInstance, "/user/hand/left/input/aim/pose")      },
        { mPoseAction, str2xrPath(mInstance, "/user/hand/right/input/aim/pose")     },
        { mFlyAction,  str2xrPath(mInstance, "/user/hand/left/input/select/click")  },
        { mFlyAction,  str2xrPath(mInstance, "/user/hand/right/input/select/click") }
    }};

    XrInteractionProfileSuggestedBinding interactionProfileSuggestedBinding{
        .type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING,
        .interactionProfile = str2xrPath(mInstance, "/interaction_profiles/khr/simple_controller"),
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

void Controllers::createAction(std::string_view actionName, std::string_view localizedActionName, XrActionType type, XrAction& action)
{
    XrActionCreateInfo actionCreateInfo{
        .type = XR_TYPE_ACTION_CREATE_INFO,
        .actionType = type,
        .countSubactionPaths = static_cast<uint32_t>(mPaths.size()),
        .subactionPaths = mPaths.data()
    };

    memcpy(actionCreateInfo.actionName, actionName.data(), actionName.length() + 1);
    memcpy(actionCreateInfo.localizedActionName, localizedActionName.data(), localizedActionName.length() + 1);

    LOGGER_XR(xrCreateAction, mActionSet, &actionCreateInfo, &action);
}
}