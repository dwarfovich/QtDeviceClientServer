#include "pch.h"
#include "main_window.h"

MainWindow::MainWindow(const std::shared_ptr<Logger>& logger, QWidget* parent)
    : QMainWindow { parent }, logger_ { logger }, server_ { nullptr, logger_ } // server_{this, logger_}
{
    Q_ASSERT(logger);

    ui.setupUi(this);
    ui.logTextEdit->setReadOnly(true);
    connect(logger_.get(), &Logger::messageAdded, this, &MainWindow::onNewLogMessage);
    logger_->logMessage("MainWindow started");

    connect(&server_, &DeviceServer::newClientConnected, this, [this](const DeviceInfo& device) {
        logger_->logMessage("new device connected");
    });

    server_.start();

    // connect(&server_, &TcpServer::newClientConnected, this, &MainWindow::onNewClientConnected);
    // server_.start(12345);
}

void MainWindow::onNewLogMessage(const QString& message)
{
    ui.logTextEdit->appendPlainText(message);
}