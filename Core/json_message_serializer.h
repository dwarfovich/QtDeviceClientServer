#pragma once

#include "messages.h"

#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
/*
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
*/
class JsonMessageSerializer
{
public:
    QByteArray serialize(const device_message::NetworkMetrics& message)
    {
        QJsonObject json = { { "type", device_message ::toString(message.type) },
                             { "bandwidth", message.bandwidth },
                             { "latency", message.latency },
                             { "packet_loss", message.packetLoss } };
        QJsonDocument document { json };

        return document.toJson(QJsonDocument::Compact);
    }

    QByteArray serialize(const device_message::StartRequest& message) {
        QJsonDocument document { QJsonObject{} };
        
        return document.toJson(QJsonDocument::Compact);
    }

    QByteArray serialize(const device_message::DeviceStatus& message)
    {
        QJsonObject json;

        json["type"]         = device_message::toString(message.type);
        json["uptime"]       = static_cast<qint64>(message.uptime);
        json["cpu_usage"]    = static_cast<int>(message.cpuUsage);
        json["memory_usage"] = static_cast<int>(message.memoryUsage);

        return QJsonDocument { json }.toJson(QJsonDocument::Compact);
    }

    QByteArray serialize(const device_message::Log& message)
    {
        QJsonObject json = { { "type", device_message::toString(message.type) },
                             { "message", message.message },
                             { "severity", device_message::toString(message.severity) } };

        QJsonDocument document { json };

        return document.toJson(QJsonDocument::Compact);
    }
};