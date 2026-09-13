#pragma once

#include "common_error.h"
#include "device_commands.h"
#include "messages.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <expected>

class JsonMessageDeserializer {
public:
    std::expected<device_message::Message, CommonError> deserialize(const QByteArray& data) const
    {
        const QJsonDocument document = QJsonDocument::fromJson(data);
        if (!document.isObject()) {
            return std::unexpected(CommonError::FailedToCreateJsonDocument);
        }

        using namespace device_message;
        const QJsonObject object = document.object();
        const auto type = toMessageType(object["type"].toString());
        switch (type) {
            case Type::NetworkMetrics:
                return Message{deserializeMessage<NetworkMetrics>(object)};
            case Type::DeviceStatus:
                return Message{deserializeMessage<device_message::DeviceStatus>(object)};
            case Type::Log:
                return Message{deserializeMessage<Log>(object)};
            case Type::StartRequest:
                return Message{StartRequest{}};
            case Type::DeviceCommand:
                return Message{deserializeDeviceCommand(object)};
            default:
                return std::unexpected(CommonError::FailedToRetreiveTypeOfMessageFromJson);
        }
    }

private:
    template <typename MessageClass>
    MessageClass deserializeMessage(const QJsonObject& object) const
    {
        MessageClass message;
        const QMetaObject& metaObject = MessageClass::staticMetaObject;
        for (int i = 0; i < metaObject.propertyCount(); ++i) {
            const QMetaProperty property = metaObject.property(i);
            const std::string& propertyName = property.name();
            const auto iter = device_message::jsonPropertyMap.find(propertyName);
            const QString& jsonName = iter != device_message::jsonPropertyMap.cend()
                                          ? QString::fromStdString(iter->second)
                                          : QString::fromLatin1(property.name());
            if (!object.contains(jsonName)) {
                continue;
            }

            auto value = object.value(jsonName).toVariant();
            if (property.name() == QByteArray{"severity"}) {
                value = QVariant::fromValue(static_cast<LogMessageSeverity>(value.toInt()));
            } else if (!value.convert(property.metaType())) {
                continue;
            }

            property.writeOnGadget(&message, value);
        }

        return message;
    }

    device_message::DeviceCommandMessage deserializeDeviceCommand(const QJsonObject& object) const
    {
        device_message::DeviceCommandMessage message;
        message.command = static_cast<DeviceCommands>(object["command"].toInt());
        message.parameter = object["parameter"].toVariant();

        return message;
    }
};