#pragma once

#include <QString>
#include <cinttypes>

namespace device_message {

enum class Type : std::uint8_t { NetworkMessage, DeviceStatus, Log, StartRequest, Unknown };

inline QString toString(Type type) {
    switch (type) {
        case Type::NetworkMessage:
            return "NetworkMessage";
        case Type::DeviceStatus:
            return "DeviceStatus";
        case Type::Log:
            return "Log";
        case Type::StartRequest:
            return "StartRequest";
        default:
            return "Unknown";
    }
}

inline Type toType(const QString& str) {
    if (str == QStringLiteral("NetworkMessage")) {
        return Type::NetworkMessage;
    } else if (str == QStringLiteral("DeviceStatus")) {
        return Type::DeviceStatus;
    } else if (str == QStringLiteral("Log")) {
        return Type::Log;
    } else if (str == QStringLiteral("StartRequest")) {
        return Type::StartRequest;
    } else {
        return Type::Unknown;
    }
}

}  // namespace device_message