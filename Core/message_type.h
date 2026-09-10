#pragma once

#include <QString>

#include <cinttypes>

namespace message {

enum class Type : std::uint8_t
{
    NetworkMessage,
    DeviceStatus,
    Log
};

QString toString(Type type)
{
    switch (type) {
        case message::Type::NetworkMessage: return "NetworkMessage";
        case message::Type::DeviceStatus: return "DeviceStatus";
        case message::Type::Log: return "Log";
        default: return "UnknownMessageType";
    }
}