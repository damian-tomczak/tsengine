#pragma once

#include "tsengine/logger.h"

#include "tsengine/ecs/ecs.h"

#include "tsengine/ecs/components/transform_component.hpp"
#include "tsengine/ecs/components/rigid_body_component.hpp"

#ifdef CYBSDK_FOUND
    #include "CVirt.h"
#endif

// TODO: create cyberith decorator
namespace ts
{
class MovementSystem : public ts::System
{
public:
    MovementSystem()
    {
        requireComponent<TransformComponent>();
        requireComponent<RigidBodyComponent>();

#ifdef CYBSDK_FOUND
        mpCyberithDevice = CybSDK::Virt::FindDevice();
        if (mpCyberithDevice == nullptr)
        {
            TS_ERR("Cyberith Virtualizer device not found");
        }

        const auto info = mpCyberithDevice->GetDeviceInfo();

        const auto virtName = info.ProductName;
        const auto virtNameLen = wcslen(virtName);
        std::vector<char> virtBuf(virtNameLen);
        wcstombs(virtBuf.data(), virtName, virtNameLen);
        std::string virtConvertedName(virtBuf.begin(), virtBuf.end());
        TS_LOG(std::format("Device found {} Firmware Version: {}.{}",
            virtConvertedName,
            static_cast<int>(info.MajorVersion),
            static_cast<int>(info.MinorVersion)).c_str());


        if (!mpCyberithDevice->Open())
        {
            TS_ERR("Unable to connect to Cyberith Virtualizer");
        }
#endif
    }

    void update(const float dt, const Controllers& controllers)
    {
        const auto player = gReg.getEntityByTag("player");

#ifdef CYBSDK_FOUND
        const auto ringHeight = mpCyberithDevice->GetPlayerHeight();
        auto ringAngle = mpCyberithDevice->GetPlayerOrientation();
        const auto movementDirection = mpCyberithDevice->GetMovementDirection();
        const auto movementSpeed = mpCyberithDevice->GetMovementSpeed();

        if (movementSpeed > 0.f)
        {
            const auto flySpeedMultiplier = player.getComponent<RigidBodyComponent>().velocity;
            ringAngle *= 2 * std::numbers::pi_v<float>;
            auto offsetX = std::sin(ringAngle) * movementSpeed * flySpeedMultiplier * dt;
            auto offsetZ = -(std::cos(ringAngle) * movementSpeed * flySpeedMultiplier * dt);

            if (movementDirection == -1.f)
            {
                offsetX *= -1;
                offsetZ *= -1;
            }

            auto& playerPosition = player.getComponent<TransformComponent>().pos;
            playerPosition.x += offsetX;
            playerPosition.z += offsetZ;
        }
#else
        for (size_t controllerIndex{}; controllerIndex < controllers.controllerCount; ++controllerIndex)
        {
            const auto flyState = controllers.getFlyState(controllerIndex);
            if (flyState)
            {
                const auto controllerPose = controllers.getPose(controllerIndex)[2];

                // TODO: something is wrong with this code, investigate it
                if ((!controllerPose.isNan()) || (controllerPose == math::Vec3{0.f}))
                {
                    const math::Vec3 forward{controllers.getPose(controllerIndex)[2]};
                    player.getComponent<TransformComponent>().pos += forward * player.getComponent<RigidBodyComponent>().velocity * dt;
                }
                else
                {
                    TS_WARN(std::format("Controller no. {} can not be located.", controllerIndex).c_str());
                }
            }
        }
#endif
    }

private:
#ifdef CYBSDK_FOUND
    CybSDK::VirtDevice* mpCyberithDevice;
#endif
};
}
