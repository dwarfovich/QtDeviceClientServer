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
            return QStringLiteral("NetworkMetrics");
        case Type::DeviceStatus:
            return QStringLiteral("DeviceStatus");
        case Type::Log:
            return QStringLiteral("Log");
        case Type::StartRequest:
            return QStringLiteral("StartRequest");
        case Type::DeviceCommand:
            return QStringLiteral("DeviceCommand");
        default:
            return QStringLiteral("Unknown");
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