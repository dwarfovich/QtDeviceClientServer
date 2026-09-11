#pragma once

#include "messages.h"

#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>

class JsonMessageSerializer
{
public:
    QByteArray serialize(const device_message::NetworkMetrics& message)
    {
        QJsonObject json = { { "type", device_message::toString(message.type) },
                             { "bandwidth", message.bandwidth },
                             { "latency", message.latency },
                             { "packet_loss", message.packetLoss } };
        QJsonDocument document { json };

        return document.toJson(QJsonDocument::Compact);
    }

    QByteArray serialize(const device_message::StartRequest& message) {
        QJsonDocument document{QJsonObject{{"type", device_message::toString(message.type)}}};
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