#pragma once

#include "ui_main_window.h"
#include "device_server.h"
#include "devices_table_model.h"

#include "Core/tcp_server.h"
#include "Core/logger.h"

#include <QtWidgets/QMainWindow>

#include <memory>

class DevicesTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const std::shared_ptr<Logger>& logger, QWidget* parent)
        : QMainWindow{parent},
          logger_{logger},
          tcpServer_{nullptr, logger_},
          devicesModel_{new DevicesTableModel{this}} {
        Q_ASSERT(logger);

        ui.setupUi(this);

        ui.clientsTableView->setModel(devicesModel_);

        ui.logTextEdit->setReadOnly(true);
        connect(logger_.get(), &Logger::messageAdded, this, &MainWindow::onNewLogMessage);
        logger_->logMessage("MainWindow started");

        connect(&tcpServer_, &DeviceServer::newClientConnected, [this](const DeviceInfo& device) {
            //devicesModel_->addDevice(device);
            logger_->logMessage("new device connected");
        });

        connect(&tcpServer_, &DeviceServer::clientDisconnected, [this](std::size_t id) {
            //devicesModel_->setDeviceDisconnected(id);
        });

        //tcpServer_.setNetworkMetricsHandler([](){});

        //onNetworkMetricsReceived
        connect(&tcpServer_, &DeviceServer::newMessageReceived, this, &MainWindow::processMessage);
        tcpServer_.start();


    }

    ~MainWindow(){
    }

private slots:
    void onNewLogMessage(const QString& message);
    void onNewClientConnected(std::size_t id){
        logger_->logMessage("New client connected, id = " + QString::number(id));
    }

        void processMessage(std::size_t clientId, const device_message::Message& message){
        logger_->logMessage(QString::number(clientId) + ": new message");
        }

   private: // data
    Ui::MainWindowClass     ui;
    std::shared_ptr<Logger> logger_;
    DeviceServer tcpServer_;
    DevicesTableModel*       devicesModel_ = nullptr;

//    TcpServer tcpServer_;
};
