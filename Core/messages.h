#pragma once

#include "message_type.h"

namespace message {

struct NetworkMetricsMessage
{
    static constexpr Type type = Type::NetworkMessage;

    double bandwidth  = 0.;
    double latency    = 0.;
    double packetLoss = 0.;
};

struct DeviceStatus
{
    static constexpr Type type = Type::DeviceStatus;

    std::size_t uptime  = 0;
    std::uint8_t cpuUsage = 0;
    std::uint8_t memoryUsage = 0;
};

enum class LogMessageSeverity : std::uint8_t{
    Info,
    Low,
    Medium,
    High,
    Critical
};

struct Log
{
    static constexpr Type type = Type::Log;

    QString message;
    LogMessageSeverity severity = LogMessageSeverity::Info;
};

}