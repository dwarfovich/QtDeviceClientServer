#pragma once

#include "device_info.h"
#include "device_status.h"

#include "Core/default_network_parameters.h"
#include "Core/json_message_deserializer.h"
#include "Core/json_message_serializer.h"
#include "Core/logger.h"
#include "Core/message_end_marker.h"
#include "Core/tcp_server.h"

#include <memory>

class DeviceServer : public QObject {
    Q_OBJECT
public:
    DeviceServer(QObject* parent, const std::shared_ptr<Logger>& logger)
        : QObject{parent}, logger_{logger}, server_{nullptr, logger}
    {
        Q_ASSERT(logger);

        connect(&server_, &TcpServer::newClientConnected, this, &DeviceServer::onNewClientConnected);
        connect(&server_, &TcpServer::clientDisconnected, this, &DeviceServer::onClientDisconnected);
        connect(&server_, &TcpServer::clientDisconnected, this, &DeviceServer::clientDisconnected);
        connect(&server_, &TcpServer::dataReceived, this, &DeviceServer::onDataReceived);
    }

    void sendMessage(std::size_t clientId, const device_message::DeviceCommandMessage& message){
        auto data = jsonSerializer_.serialize(message) + device_message::dataEndMarker;
        server_.sendMessage(clientId, std::move(data));
    }

public slots:
    void start()
    {
        server_.start(default_network_parameters::serverPort);
    }

    void stop()
    {
        server_.stop();
    }

signals:
    void newClientConnected(const DeviceInfo& device);
    void clientDisconnected(std::size_t id);
    void newMessageReceived(std::size_t clientId, const device_message::Message& message);

private slots:
    void onNewClientConnected(std::size_t id, const QHostAddress& address)
    {
        DeviceInfo device{id, address, DeviceStatus::Connected};
        server_.sendMessage(id,
                            jsonSerializer_.serialize(device_message::StartRequest{}) + device_message::dataEndMarker);
        emit newClientConnected(device);
    }

    void onClientDisconnected(std::size_t id)
    {
        currentMessages_.erase(id);
    }

    void onDataReceived(std::size_t clientId, const QByteArray& newData)
    {
        auto [iter, inserted] = currentMessages_.try_emplace(clientId, newData);
        if (!inserted) {
            iter->second += newData;
        }
        const auto& data = iter->second;
        if (data.endsWith(device_message::dataEndMarker)) {
            logger_->messageAdded(data);
            processReceivedMessage(clientId, data);
        }
    }

private:  // methods
    void processReceivedMessage(std::size_t clientId, const QByteArray& data)
    {
        //try{
        const auto& message = jsonDeserializer_.deserialize(data);
        currentMessages_.erase(clientId);
        emit newMessageReceived(clientId, message);
        //} catch(...){
        //}
    }

private:  // data
    std::shared_ptr<Logger> logger_ = nullptr;
    TcpServer server_;
    std::unordered_map<std::size_t, QByteArray> currentMessages_;
    JsonMessageDeserializer jsonDeserializer_;
    JsonMessageSerializer jsonSerializer_;
};