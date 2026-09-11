#pragma once

#include <memory>

#include "Core/json_message_deserializer.h"
#include "Core/logger.h"
#include "Core/message_end_marker.h"
#include "Core/tcp_server.h"
#include "device_info.h"
#include "device_status.h"

class DeviceServer : public QObject {
    Q_OBJECT
   public:
    DeviceServer(QObject* parent, const std::shared_ptr<Logger>& logger)
        : QObject{parent}, logger_{logger}, tcpServer_{nullptr, logger} {
        connect(&tcpServer_, &TcpServer::newClientConnected, this, &DeviceServer::onNewClientConnected);
        connect(&tcpServer_, &TcpServer::clientDisconnected, this, &DeviceServer::clientDisconnected);
        connect(&tcpServer_, &TcpServer::dataReceived, this, &DeviceServer::onDataReceived);
    }

    //void setNetworkMetricsHandler(
    //    std::function<void(std::size_t clientId, const device_message::NetworkMetrics&)> handler) {
    //    networkMetricsHandler_ = handler;
    //}
    //void setDeviceStatusHandler(std::function<void(std::size_t clientId, const device_message::DeviceStatus&)> handler) {
    //    deviceStatusHandler_ = handler;
    //}
    //void setLogHandler(std::function<void(std::size_t clientId, const device_message::Log&)> handler) {
    //    logHandler_ = handler;
    //}

   public slots:
    void start() {
        tcpServer_.start(defaultPort_);
    }
    void stop() {
        tcpServer_.stop();
    }

   signals:
    void newClientConnected(const DeviceInfo& device);
    void clientDisconnected(std::size_t id);

   private slots:
    void onNewClientConnected(std::size_t id, const QHostAddress& address) {
        DeviceInfo device{id, address, DeviceStatus::Connected};
        emit newClientConnected(device);
    }
    // TODO: Clear current message on diconnect;

    void onDataReceived(std::size_t clientId, QByteArray newData) {
        auto [iter, inserted] = currentMessages_.try_emplace(clientId, newData);
        if (!inserted) {
            iter->second += newData;
        }
        const auto& data = iter->second;
        if (data.endsWith(device_message::dataEndMarker)) {
            processReceivedMessage(clientId, data);
        }
        // tcpServer_.sendMessage(clientId, "Hello from server!" + device_message::dataEndMarker);
        tcpServer_.sendMessage(clientId, "{\"type\":\"start_request\"}" + device_message::dataEndMarker);
    }

   private:  // methods
    void processReceivedMessage(std::size_t clientId, const QByteArray& data) {
        qDebug() << "Received message from " + QString::number(clientId) + ": " + data;
        // call handler
        currentMessages_.erase(clientId);
    }

   private:  // data
    std::shared_ptr<Logger> logger_ = nullptr;
    TcpServer tcpServer_;
    const quint16 defaultPort_ = 12345;
    std::unordered_map<std::size_t, QByteArray> currentMessages_;
    JsonMessageDeserializer jsonDeserializer_;
    //std::function<void(std::size_t clientId, const device_message::StartRequest&)> startRequestHandler_;
    //std::function<void(std::size_t clientId, const device_message::NetworkMetrics&)> networkMetricsHandler_;
    //std::function<void(std::size_t clientId, const device_message::DeviceStatus&)> deviceStatusHandler_;
    //std::function<void(std::size_t clientId, const device_message::Log&)> logHandler_;
};