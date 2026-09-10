#pragma once

#include "messages.h"

#include <QByteArray>

class JsonMessageDeserializer
{
public:
    void deserialize(const QByteArray& data)
    {
        const auto document = QJsonDocument::fromJson(data);
        const auto object   = document.object();

        const auto type = object["type"].toString();

        if (type == "start_request") {
            device_message::StartRequest message{ };
            startRequestHandler_(message);
        }
    }

    void setNetworkMetricsHandler(std::function<void(const device_message::NetworkMetrics&)> handler);
    void setDeviceStatusHandler(std::function<void(const device_message::DeviceStatus&)> handler);
    void setLogHandler(std::function<void(const device_message::Log&)> handler);
    void setStartRequestHandler(std::function<void(const device_message::StartRequest&)> handler){
        startRequestHandler_ = handler;
    }

private:
    std::function<void(const device_message::StartRequest&)> startRequestHandler_;
};