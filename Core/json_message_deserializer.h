#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "messages.h"

class JsonMessageDeserializer {
public:
    device_message::Message deserialize(const QByteArray& data) const
    {
        const QJsonDocument document = QJsonDocument::fromJson(data);

        if (!document.isObject()) {
            throw std::runtime_error("JSON message is not an object");
        }

        const QJsonObject object = document.object();
        auto d = object["type"].toString();
        const auto type = device_message::toMessageType(object["type"].toString());

        switch (type) {
            case device_message::Type::NetworkMetrics:
                return device_message::Message{deserializeMessage<device_message::NetworkMetrics>(object)};
            case device_message::Type::DeviceStatus:
                return device_message::Message{deserializeMessage<device_message::DeviceStatus>(object)};
            case device_message::Type::Log:
                return device_message::Message{deserializeMessage<device_message::Log>(object)};
            case device_message::Type::StartRequest:
                return device_message::Message{device_message::StartRequest{}};
            default:
                throw std::runtime_error("Unknown message type");
        }
    }

private:
    template <typename T>
    T deserializeMessage(const QJsonObject& object) const
    {
        T message;
        const QMetaObject& metaObject = T::staticMetaObject;

        for (int i = 0; i < metaObject.propertyCount(); ++i) {
            const QMetaProperty property = metaObject.property(i);
            const std::string& propertyName = property.name();
            const auto iter = device_message::jsonPropertyMap.find(propertyName);
            const QString& jsonName = iter != device_message::jsonPropertyMap.end()
                                          ? QString::fromStdString(iter->second)
                                          : QString::fromLatin1(property.name());
            // TODO: Handle errors.
            if (!object.contains(jsonName)) {
                continue;
            }

            auto value = object.value(jsonName).toVariant();
            if (!value.convert(property.metaType())) {
                continue;
            }

            property.writeOnGadget(&message, value);
        }

        return message;
    }
};