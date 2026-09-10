#include "pch.h"
#include "main_window.h"
#include "devices_table_model.h"

MainWindow::MainWindow(const std::shared_ptr<Logger>& logger, QWidget* parent)
    : QMainWindow { parent }, logger_ { logger }, server_ { nullptr, logger_ }, devicesModel_{new DevicesTableModel {this}}
{
    Q_ASSERT(logger);

    ui.setupUi(this);

    ui.clientsTableView->setModel(devicesModel_);
    //connect()

    ui.logTextEdit->setReadOnly(true);
    connect(logger_.get(), &Logger::messageAdded, this, &MainWindow::onNewLogMessage);
    logger_->logMessage("MainWindow started");

    connect(&server_, &DeviceServer::newClientConnected, [this](const DeviceInfo& device) {
        devicesModel_->addDevice(device);
        logger_->logMessage("new device connected");
    });

    connect(&server_, &DeviceServer::clientDisconnected, [this](std::size_t id) {
        devicesModel_->setDeviceDisconnected(id);
    });

    server_.start();

    // connect(&server_, &TcpServer::newClientConnected, this, &MainWindow::onNewClientConnected);
    // server_.start(12345);
}

void MainWindow::onNewLogMessage(const QString& message)
{
    ui.logTextEdit->appendPlainText(message);
}