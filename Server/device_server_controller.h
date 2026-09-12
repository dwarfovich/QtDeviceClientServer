#pragma once

#include "data_table_model.h"
#include "device_server.h"
#include "devices_table_model.h"

#include "Core/logger.h"

#include <QObject>

class DeviceServerController : public QObject {
    Q_OBJECT

public:
    DeviceServerController(QObject* parent, const std::shared_ptr<Logger>& logger)
        : QObject{parent},
          logger_{logger},
          server_{nullptr, logger},
          devicesModel_{new DevicesTableModel{this}},
          dataModel_{new DataTableModel{this}}
    {
        connect(&server_, &DeviceServer::newClientConnected, this, &DeviceServerController::onNewClientConnected);
        connect(&server_, &DeviceServer::clientDisconnected, this, &DeviceServerController::onClientDisconnected);
        connect(&server_, &DeviceServer::newMessageReceived, this, &DeviceServerController::processMessage);

        server_.start();
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

    void enableDeviceSignalLamp(std::size_t id, bool newState)
    {
        server_.sendMessage(id, device_message::DeviceCommandMessage{DeviceCommands::EnableSignalLamp, newState});
    }

    void startDevice(std::size_t id)
    {
        server_.sendMessage(id, device_message::DeviceCommandMessage{DeviceCommands::StartStop, true});
    }
    void stopoDevice(std::size_t id)
    {
        server_.sendMessage(id, device_message::DeviceCommandMessage{DeviceCommands::StartStop, false});
    }

    void startAllDevices()
    {
        for (const auto& id : dataModel_->ids()) {
            server_.sendMessage(id, device_message::DeviceCommandMessage{DeviceCommands::StartStop, true});
        }
    }
    void stopAllDevices()
    {
        for (const auto& id : dataModel_->ids()) {
            server_.sendMessage(id, device_message::DeviceCommandMessage{DeviceCommands::StartStop, false});
        }
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
        try{
        const auto& data = as<device_message::Log>(message);
        logger_->logMessage(data.message, clientId, data.severity);
        } catch(...){}
    }

private:
    std::shared_ptr<Logger> logger_ = nullptr;
    DeviceServer server_;
    DevicesTableModel* devicesModel_ = nullptr;
    DataTableModel* dataModel_ = nullptr;
};