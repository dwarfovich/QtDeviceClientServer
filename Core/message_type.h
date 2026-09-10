#pragma once

#include <QString>

#include <cinttypes>

namespace device_message {

enum class Type : std::uint8_t
{
    NetworkMessage,
    DeviceStatus,
    Log,
    StartRequest
};

inline QString toString(Type type)
{
    switch (type) {
        case Type::NetworkMessage: return "NetworkMessage";
        case Type::DeviceStatus: return "DeviceStatus";
        case Type::Log: return "Log";
        default: return "Unknown";
    }
}

} // namespace device_message