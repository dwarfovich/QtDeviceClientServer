#pragma once

#include "device_info.h"
#include "device_status.h"

#include "Core/tcp_server.h"
#include "Core/logger.h"
#include "Core/message_end_marker.h"

#include <memory>

class DeviceServer : public QObject
{
    Q_OBJECT
public:
    DeviceServer(QObject* parent, const std::shared_ptr<Logger>& logger)
        : QObject { parent }, logger_ { logger }, server_ { nullptr, logger }
    {
        connect(&server_, &TcpServer::newClientConnected, this, &DeviceServer::onNewClientConnected);
        connect(&server_, &TcpServer::clientDisconnected, this, &DeviceServer::clientDisconnected);
        connect(&server_, &TcpServer::dataReceived, this, &DeviceServer::onDataReceived);
    }

public slots:
    void start() { server_.start(defaultPort_); }
    void stop() { server_.stop(); }

signals:
    void newClientConnected(const DeviceInfo& device);
    void clientDisconnected(std::size_t id);

private slots:
    void onNewClientConnected(std::size_t id, const QHostAddress& address)
    {
        DeviceInfo device { id, address, DeviceStatus::Connected };
        emit       newClientConnected(device);
    }
    // TODO: Clear current message on diconnect;

    void onDataReceived(std::size_t clientId, QByteArray newData) {
        auto [iter, inserted] = currentMessages_.try_emplace(clientId, newData);
        if (!inserted) {
            iter->second += newData;
        }
        const auto& data = iter->second;
        if (data.endsWith(messageEndMarker)){
            processReceivedMessage(clientId, data);
        }
    }

    private: // methods
        void processReceivedMessage(std::size_t clientId, const QByteArray& data){
            qDebug() << "Received message from " + QString::number(clientId) + ": " + data;

            currentMessages_.erase(clientId);
        }

private: // data
    std::shared_ptr<Logger> logger_;
    TcpServer               server_;
    const quint16           defaultPort_ = 12345;
    std::unordered_map<std::size_t, QByteArray> currentMessages_;
};