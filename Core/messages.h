#pragma once

#include "message_type.h"

namespace device_message {

struct NetworkMetrics
{
    static constexpr Type type = Type::NetworkMessage;

    double bandwidth  = 0.;
    double latency    = 0.;
    double packetLoss = 0.;
};

struct DeviceStatus
{
    static constexpr Type type = Type::DeviceStatus;

    std::size_t  uptime      = 0;
    std::uint8_t cpuUsage    = 0;
    std::uint8_t memoryUsage = 0;
};

enum class LogMessageSeverity : std::uint8_t
{
    Info,
    Low,
    Medium,
    High,
    Critical
};

inline QString toString(LogMessageSeverity severity){
    switch (severity) {
        case LogMessageSeverity::Info: return "INFO";
        case LogMessageSeverity::Low: return "LOW";
        case LogMessageSeverity::Medium: return "MEDIUM";
        case LogMessageSeverity::High: return "HIGH";
        case LogMessageSeverity::Critical: return "CRITICAL";
        default: return "Unknown";
    };
}

struct Log
{
    static constexpr Type type = Type::Log;

    QString            message;
    LogMessageSeverity severity = LogMessageSeverity::Info;
};

struct StartRequest
{
    static constexpr Type type = Type::Log;
};

} // namespace device_message