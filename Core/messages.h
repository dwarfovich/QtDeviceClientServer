#pragma once

#include "message_type.h"

namespace device_message {

struct NetworkMetrics {
    static constexpr Type type = Type::NetworkMessage;

    double bandwidth = 0.;
    double latency = 0.;
    double packetLoss = 0.;
};

struct DeviceStatus {
    static constexpr Type type = Type::DeviceStatus;

    std::size_t uptime = 0;
    std::uint8_t cpuUsage = 0;
    std::uint8_t memoryUsage = 0;
};

enum class LogMessageSeverity : std::uint8_t { Info, Low, Medium, High, Critical, Unknown };

inline QString toString(LogMessageSeverity severity) {
    switch (severity) {
        case LogMessageSeverity::Info:
            return "INFO";
        case LogMessageSeverity::Low:
            return "LOW";
        case LogMessageSeverity::Medium:
            return "MEDIUM";
        case LogMessageSeverity::High:
            return "HIGH";
        case LogMessageSeverity::Critical:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    };
}

inline LogMessageSeverity toLogMessageSeverity(QString str) {
    if (str == QStringLiteral("INFO")) {
        return LogMessageSeverity::Info;
    } else if (str == QStringLiteral("LOW")) {
        return LogMessageSeverity::Low;
    } else if (str == QStringLiteral("MEDIUM")) {
        return LogMessageSeverity::Medium;
    } else if (str == QStringLiteral("HIGH")) {
        return LogMessageSeverity::High;
    } else if (str == QStringLiteral("CRITICAL")) {
        return LogMessageSeverity::Critical;
    } else {
        return LogMessageSeverity::Unknown;
    }
}

struct Log {
    static constexpr Type type = Type::Log;

    QString message;
    LogMessageSeverity severity = LogMessageSeverity::Info;
};

struct StartRequest {
    static constexpr Type type = Type::StartRequest;
};

}  // namespace device_message