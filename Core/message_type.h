#pragma once

#include "messages.h"

#include <QString>
#include <cinttypes>

namespace device_message {

enum class Type : std::uint8_t {
    NetworkMetrics,
    DeviceStatus,
    Log,
    StartRequest,
    DeviceCommand,
    Unknown
};

inline QString toString(Type type)
{
    switch (type) {
        case Type::NetworkMetrics:
            return "NetworkMetrics";
        case Type::DeviceStatus:
            return "DeviceStatus";
        case Type::Log:
            return "Log";
        case Type::StartRequest:
            return "StartRequest";
        case Type::DeviceCommand:
            return "DeviceCommand";
        default:
            return "Unknown";
    }
}

inline Type toMessageType(const QString& str)
{
    if (str == QStringLiteral("NetworkMetrics")) {
        return Type::NetworkMetrics;
    } else if (str == QStringLiteral("DeviceStatus")) {
        return Type::DeviceStatus;
    } else if (str == QStringLiteral("Log")) {
        return Type::Log;
    } else if (str == QStringLiteral("StartRequest")) {
        return Type::StartRequest;
    } else if (str == QStringLiteral("DeviceCommand")) {
        return Type::DeviceCommand;
    } else {
        return Type::Unknown;
    }
}

}  // namespace device_message