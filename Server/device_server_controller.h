#pragma once

#include "device_server.h"
#include "devices_table_model.h"
#include "data_table_model.h"

#include "Core/logger.h"

#include <QObject>

class DeviceServerController : public QObject {
public:
    DeviceServerController(QObject* parent, const std::shared_ptr<Logger>& logger)
        : QObject{parent}, logger_{logger}, server_{nullptr, logger}, devicesModel_{new DevicesTableModel{this}}, dataModel_{new DataTableModel {this}}
    {
        connect(&server_, &DeviceServer::newClientConnected, this, &DeviceServerController::onNewClientConnected);
        connect(&server_, &DeviceServer::clientDisconnected, this, &DeviceServerController::onClientDisconnected);
        connect(&server_, &DeviceServer::newMessageReceived, this, &DeviceServerController::processMessage);

        server_.start();
        //logger->logMessage("App started");
    }

    ~DeviceServerController()
    {
        server_.stop();
    }

    DevicesTableModel* devicesModel() const
    {
        return devicesModel_;
    }

        DataTableModel* dataModel() const
    {
        return dataModel_;
    }

private slots:
    void onNewClientConnected(const DeviceInfo& device)
    {
        logger_->logMessage("New client connected, id = " + QString::number(device.id));
        devicesModel_->addDevice(device);
        dataModel_->addDevice(device.id);
    }

    void onClientDisconnected(std::size_t id)
    {
        logger_->logMessage("Client " + QString::number(id) + " disconnected");
        devicesModel_->setDeviceDisconnected(id);
        dataModel_->removeDevice(id);
    }

    void processMessage(std::size_t clientId, const device_message::Message& message)
    {
        dataModel_->updateDevice(clientId, message);
    }

private:
    std::shared_ptr<Logger> logger_ = nullptr;
    DeviceServer server_;
    DevicesTableModel* devicesModel_ = nullptr;
    DataTableModel* dataModel_ = nullptr;
};