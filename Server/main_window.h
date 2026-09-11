#pragma once

#include "device_server.h"
#include "devices_table_model.h"
#include "ui_main_window.h"

#include "Core/logger.h"

#include <QtWidgets/QMainWindow>

#include <memory>

class DevicesTableModel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(const std::shared_ptr<Logger>& logger, QWidget* parent)
        : QMainWindow{parent}, logger_{logger}, server_{nullptr, logger_}, devicesModel_{new DevicesTableModel{this}} {
        Q_ASSERT(logger);

        ui.setupUi(this);
        setupWindowGeometry();

        ui.clientsTableView->setModel(devicesModel_);

        ui.logTextEdit->setReadOnly(true);
        connect(logger_.get(), &Logger::messageAdded, this, &MainWindow::onNewLogMessage);
        connect(&server_, &DeviceServer::newClientConnected, this, &MainWindow::onNewClientConnected);
        connect(&server_, &DeviceServer::clientDisconnected, this, &MainWindow::onClientDisconnected);
        connect(&server_, &DeviceServer::newMessageReceived, this, &MainWindow::processMessage);

        server_.start();
        logger->logMessage("App started");
    }

    ~MainWindow() {
        server_.stop();
    }

private slots:
    void onNewLogMessage(const QString& message) {
        ui.logTextEdit->appendPlainText(message);
    }

    void onNewClientConnected(const DeviceInfo& device) {
        logger_->logMessage("New client connected, id = " + QString::number(device.id));
        devicesModel_->addDevice(device);
    }

    void onClientDisconnected(std::size_t id) {
        logger_->logMessage("Client " + QString::number(id) + " disconnected");
        devicesModel_->setDeviceDisconnected(id);
    }

    void processMessage(std::size_t clientId, const device_message::Message& message) {
        // logger_->logMessage(QString::number(clientId) + ": new message");
    }

private:  // methods
    void setupWindowGeometry() {
        QScreen* screen = QGuiApplication::primaryScreen();
        const QRect geometry = screen->availableGeometry();
        const int width = geometry.width() * 0.7;
        const int height = geometry.height() * 0.6;
        resize(width, height);
        move(geometry.x() + (geometry.width() - width) / 2, geometry.y() + (geometry.height() - height) / 2);
    }

private:  // data
    Ui::MainWindowClass ui;
    std::shared_ptr<Logger> logger_ = nullptr;
    DeviceServer server_;
    DevicesTableModel* devicesModel_ = nullptr;
};
