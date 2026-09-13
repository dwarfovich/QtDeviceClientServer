#pragma once

#include "device_commands.h"
#include "log_message_severity.h"
#include "message_type.h"

#include <QMetaProperty>
#include <QString>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

namespace device_message {

inline const QByteArray dataEndMarker = "\r\n\r\n";

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
    Q_PROPERTY(bool signalLampState MEMBER signalLampState)
    Q_PROPERTY(bool isActive MEMBER isActive)
    Q_PROPERTY(double memoryUsage MEMBER memoryUsage)

public:
    static constexpr Type type = Type::DeviceStatus;

    int uptime = 0;
    int cpuUsage = 0;
    bool signalLampState = false;
    bool isActive = true;
    double memoryUsage = 0.0;
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

class StartRequest {
public:
    static constexpr Type type = Type::StartRequest;
};

class DeviceCommandMessage {
    Q_GADGET

    Q_PROPERTY(DeviceCommands command MEMBER command)
    Q_PROPERTY(QVariant parameter MEMBER parameter)

public:
    static constexpr Type type = Type::DeviceCommand;

    DeviceCommands command = DeviceCommands::EnableSignalLamp;
    QVariant parameter;
};

using MessageData = std::variant<NetworkMetrics, DeviceStatus, Log, StartRequest, DeviceCommandMessage>;

class Message {
public:
    template <typename T>
    explicit Message(T data) : type_(T::type), data_(std::move(data))
    {
    }

    Type type() const
    {
        return type_;
    }

    const MessageData& data() const
    {
        return data_;
    }

    MessageData& data()
    {
        return data_;
    }

private:
    Type type_;
    MessageData data_;
};

template <typename T>
const T& as(const Message& message)
{
    Q_ASSERT(message.type() == T::type);

    return std::get<T>(message.data());
}

template <typename T>
T& as(Message& message)
{
    Q_ASSERT(message.type() == T::type);

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
    {"signalLampState", "signal_lamp_state"},
    {"isActive", "is_active"},

    {"message", "message"},
    {"severity", "severity"},

    {"command", "command"},
    {"parameter", "parameter"},
};

}  // namespace device_message