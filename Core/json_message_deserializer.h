#pragma once

#include <QByteArray>

#include "messages.h"

class JsonMessageDeserializer {
   public:
    device_message::Message deserialize(const QByteArray& data) const {
        const QJsonDocument document = QJsonDocument::fromJson(data);

        if (!document.isObject()) {
            throw std::runtime_error("JSON message is not an object");
        }

        const QJsonObject object = document.object();

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

    //void deserialize(const QByteArray& data) {
    //    using namespace device_message;

    //    const auto document = QJsonDocument::fromJson(data);
    //    const auto object = document.object();
    //    const auto type = toType(object["type"].toString());

    //    switch (type) {
    //        case Type::StartRequest: {
    //            if (startRequestHandler_) {
    //                startRequestHandler_(StartRequest{});
    //            }
    //            break;
    //        }
    //        case Type::NetworkMessage: {
    //            NetworkMetrics message{.bandwidth = object["bandwidth"].toDouble(),
    //                                   .latency = object["latency"].toDouble(),
    //                                   .packetLoss = object["packet_loss"].toDouble()};
    //            if (networkMetricsHandler_) {
    //                networkMetricsHandler_(std::move(message));
    //            }
    //            break;
    //        }
    //        case Type::DeviceStatus: {
    //            // TODO: Properly handle integer values.
    //            device_message::DeviceStatus message{/*.uptime = object["bandwidth"].toInteger(),
    //                                   .cpuUsage= object["cpu_usage"].toInteger(),
    //                                   .memoryUsage = object["memory_usage"].toDouble()*/};
    //            if (deviceStatusHandler_) {
    //                deviceStatusHandler_(std::move(message));
    //            }
    //            break;
    //        }
    //        case Type::Log: {
    //            // TODO: Properly handle integer values.
    //            Log message{.message = object["message"].toString(), .severity = {}};
    //            if (logHandler_) {
    //                logHandler_(std::move(message));
    //            }
    //            break;
    //        }
    //    }
    //}

    void setNetworkMetricsHandler(std::function<void(const device_message::NetworkMetrics&)> handler) {
        networkMetricsHandler_ = handler;
    }
    void setDeviceStatusHandler(std::function<void(const device_message::DeviceStatus&)> handler) {
        deviceStatusHandler_ = handler;
    }
    void setLogHandler(std::function<void(const device_message::Log&)> handler) {
        logHandler_ = handler;
    }
    void setStartRequestHandler(std::function<void(const device_message::StartRequest&)> handler) {
        startRequestHandler_ = handler;
    }

   private:
    template <typename T>
    T deserializeMessage(const QJsonObject& object) const {
        T message;

        const QMetaObject& metaObject = T::staticMetaObject;

        for (int i = 0; i < metaObject.propertyCount(); ++i) {
            const QMetaProperty property = metaObject.property(i);

            const std::string propertyName = property.name();

            const auto it = device_message::jsonPropertyMap.find(propertyName);

            const QString jsonName = it != device_message::jsonPropertyMap.end() ? QString::fromStdString(it->second)
                                                                                 : QString::fromLatin1(property.name());

            if (!object.contains(jsonName)) {
                continue;
            }

            QVariant value = object.value(jsonName).toVariant();

            if (!value.convert(property.metaType())) {
                continue;
            }

            property.writeOnGadget(&message, value);
        }

        return message;
    }

    std::function<void(const device_message::StartRequest&)> startRequestHandler_;
    std::function<void(const device_message::NetworkMetrics&)> networkMetricsHandler_;
    std::function<void(const device_message::DeviceStatus&)> deviceStatusHandler_;
    std::function<void(const device_message::Log&)> logHandler_;
};