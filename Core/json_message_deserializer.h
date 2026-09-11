#pragma once

#include <QByteArray>

#include "messages.h"

class JsonMessageDeserializer {
   public:
    void deserialize(const QByteArray& data) {
        using namespace device_message;

        const auto document = QJsonDocument::fromJson(data);
        const auto object = document.object();
        const auto type = toType(object["type"].toString());

        switch (type) {
            case Type::StartRequest: {
                if (startRequestHandler_) {
                    startRequestHandler_(StartRequest{});
                }
                break;
            }
            case Type::NetworkMessage: {
                NetworkMetrics message{.bandwidth = object["bandwidth"].toDouble(),
                                       .latency = object["latency"].toDouble(),
                                       .packetLoss = object["packet_loss"].toDouble()};
                if (networkMetricsHandler_) {
                    networkMetricsHandler_(std::move(message));
                }
                break;
            }
            case Type::DeviceStatus: {
                // TODO: Properly handle integer values.
                device_message::DeviceStatus message{/*.uptime = object["bandwidth"].toInteger(),
                                       .cpuUsage= object["cpu_usage"].toInteger(),
                                       .memoryUsage = object["memory_usage"].toDouble()*/};
                if (deviceStatusHandler_) {
                    deviceStatusHandler_(std::move(message));
                }
                break;
            }
            case Type::Log: {
                // TODO: Properly handle integer values.
                Log message{.message = object["message"].toString(), .severity = {}};
                if (logHandler_) {
                    logHandler_(std::move(message));
                }
                break;
            }
        }
    }

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
    std::function<void(const device_message::StartRequest&)> startRequestHandler_;
    std::function<void(const device_message::NetworkMetrics&)> networkMetricsHandler_;
    std::function<void(const device_message::DeviceStatus&)> deviceStatusHandler_;
    std::function<void(const device_message::Log&)> logHandler_;
};