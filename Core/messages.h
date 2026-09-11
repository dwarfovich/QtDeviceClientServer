#pragma once

#include <QMetaProperty>
#include <QString>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

#include "message_type.h"

namespace device_message {

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
    }
}

inline LogMessageSeverity toLogMessageSeverity(const QString& str) {
    if (str == QStringLiteral("INFO")) {
        return LogMessageSeverity::Info;
    }
    if (str == QStringLiteral("LOW")) {
        return LogMessageSeverity::Low;
    }
    if (str == QStringLiteral("MEDIUM")) {
        return LogMessageSeverity::Medium;
    }
    if (str == QStringLiteral("HIGH")) {
        return LogMessageSeverity::High;
    }
    if (str == QStringLiteral("CRITICAL")) {
        return LogMessageSeverity::Critical;
    }

    return LogMessageSeverity::Unknown;
}

class NetworkMetrics {
    Q_GADGET

    Q_PROPERTY(double bandwidth MEMBER bandwidth)
    Q_PROPERTY(double latency MEMBER latency)
    Q_PROPERTY(double packetLoss MEMBER packetLoss)

   public:
    static constexpr typename device_message::Type type = device_message::Type::NetworkMetrics;

    double bandwidth = 0.0;
    double latency = 0.0;
    double packetLoss = 0.0;
};

class DeviceStatus {
    Q_GADGET

    Q_PROPERTY(int uptime MEMBER uptime)
    Q_PROPERTY(int cpuUsage MEMBER cpuUsage)
    Q_PROPERTY(double memoryUsage MEMBER memoryUsage)

   public:
    static constexpr Type type = Type::DeviceStatus;

    int uptime = 0;
    int cpuUsage = 0;
    int memoryUsage = 0;
};

class Log {
    Q_GADGET

    Q_PROPERTY(QString message MEMBER message)
    Q_PROPERTY(LogMessageSeverity severity MEMBER severity)

   public:
    static constexpr Type type = Type::Log;

    QString message;
    LogMessageSeverity severity = LogMessageSeverity::Unknown;
};

struct StartRequest {
    static constexpr Type type = Type::StartRequest;
};

using MessageData = std::variant<NetworkMetrics, DeviceStatus, Log, StartRequest>;

class Message {
   public:
    template <typename T>
    explicit Message(T data) : type_(T::type), data_(std::move(data)) {
    }

    Type type() const {
        return type_;
    }

    const MessageData& data() const {
        return data_;
    }

    MessageData& data() {
        return data_;
    }

   private:
    Type type_;
    MessageData data_;
};

template <typename T>
const T& as(const Message& message) {
    if (message.type() != T::type) {
        throw std::bad_cast{};
    }

    return std::get<T>(message.data());
}

template <typename T>
T& as(Message& message) {
    if (message.type() != T::type) {
        throw std::bad_cast{};
    }

    return std::get<T>(message.data());
}

using JsonPropertyMap = std::unordered_map<std::string, std::string>;

inline const JsonPropertyMap jsonPropertyMap{
    {"bandwidth", "bandwidth"},
    {"latency", "latency"},
    {"packetLoss", "packet_loss"},

    {"uptime", "uptime"},
    {"cpuUsage", "cpu_usage"},
    {"memoryUsage", "memory_usage"},

    {"message", "message"},
    {"severity", "severity"},
};

}  // namespace device_message