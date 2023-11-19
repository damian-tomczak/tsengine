#pragma once

#include "tsengine/ecs/ecs.h"

#include <string>

struct EchoComponent : public ts::Component
{
    std::string message;
    size_t messageId{};

    EchoComponent(const std::string_view message_ = "default echo message") :
        message{message_}
    {
        messageId = std::hash<std::string>{}(message);
    }
};