#include "pch.h"
#include "main_window.h"

MainWindow::MainWindow(const std::shared_ptr<Logger>& logger, QWidget* parent)
    : QMainWindow { parent }, logger_ { logger }, server_{this, logger_}
{
    Q_ASSERT(logger);

    ui.setupUi(this);
    ui.logTextEdit->setReadOnly(true);
    connect(logger_.get(), &Logger::messageAdded, this, &MainWindow::onNewLogMessage, Qt::QueuedConnection);

    logger_->logMessage("MainWindow started");
    server_.start(12345);
}

void MainWindow::onNewLogMessage(const QString& message)
{
    ui.logTextEdit->appendPlainText(message);
}