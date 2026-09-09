#pragma once

#include "device_info.h"
#include "device_status.h"

#include "Core/tcp_server.h"
#include "Core/logger.h"

#include <memory>

class DeviceServer : public QObject
{
    Q_OBJECT
public:
    DeviceServer(QObject* parent, const std::shared_ptr<Logger>& logger) : QObject { parent }, logger_ { logger }, server_{nullptr, logger} {
        connect(&server_, &TcpServer::newClientConnected, this, &DeviceServer::onNewClientConnected);
    }

public slots:
    void start() { server_.start(defaultPort_); }
    void stop() { server_.stop(); }

signals:
    void newClientConnected(const DeviceInfo& device);

    private slots:
        void onNewClientConnected(std::size_t id, const QHostAddress& address){
            DeviceInfo device {id, address, DeviceStatus::Connected};
            emit newClientConnected(device);
        }
private:
    std::shared_ptr<Logger> logger_;
    TcpServer               server_;
    const quint16           defaultPort_ = 12345;
};