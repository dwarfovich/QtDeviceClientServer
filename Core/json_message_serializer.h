#pragma once

#include "messages.h"
#include "utilities.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

#define JSON_FIELD(property) QString::fromStdString(device_message::jsonPropertyMap.at(#property)), property

#define CONVERTED_JSON_FIELD(property, expression) \
    QString::fromStdString(device_message::jsonPropertyMap.at(#property)), expression

class JsonMessageSerializer {
public:
    QJsonObject makeBaseJson(device_message::Type type)
    {
        return {{"type", device_message::toString(type)}};
    }

    QByteArray serialize(const device_message::NetworkMetrics& message)
    {
        QJsonObject json = makeBaseJson(message.type);
        json.insert(JSON_FIELD(message.bandwidth));
        json.insert(JSON_FIELD(message.latency));
        json.insert(JSON_FIELD(message.packetLoss));

        return serialize(json);
    }

    QByteArray serialize(const device_message::StartRequest& message)
    {
        return serialize(makeBaseJson(message.type));
    }

    QByteArray serialize(const device_message::DeviceStatus& message)
    {
        QJsonObject json = makeBaseJson(message.type);
        json.insert(JSON_FIELD(message.uptime));
        json.insert(JSON_FIELD(message.cpuUsage));
        json.insert(JSON_FIELD(message.memoryUsage));
        json.insert(JSON_FIELD(message.signalLampState));
        json.insert(JSON_FIELD(message.isActive));

        return serialize(json);
    }

    QByteArray serialize(const device_message::Log& message)
    {
        QJsonObject json = makeBaseJson(message.type);
        json.insert(JSON_FIELD(message.message));
        json.insert(CONVERTED_JSON_FIELD(message.severity, toUnderlyingType(message.severity)));

        return serialize(json);
    }

    QByteArray serialize(const device_message::DeviceCommandMessage& message)
    {
        QJsonObject json = makeBaseJson(message.type);
        json.insert(CONVERTED_JSON_FIELD(command, toUnderlyingType(message.command)));
        json.insert(CONVERTED_JSON_FIELD(parameter, QJsonValue::fromVariant(message.parameter)));

        return serialize(json);
    }

private:
    QByteArray serialize(const QJsonObject& json)
    {
        return QJsonDocument{json}.toJson(QJsonDocument::Compact);
    }
};