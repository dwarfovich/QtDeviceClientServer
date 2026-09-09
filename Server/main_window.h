#pragma once

#include "ui_main_window.h"
#include "device_server.h"

#include "Core/tcp_server.h"
#include "Core/logger.h"

#include <QtWidgets/QMainWindow>

#include <memory>

class DevicesTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const std::shared_ptr<Logger>& logger, QWidget* parent = nullptr);
    ~MainWindow(){
    }

private slots:
    void onNewLogMessage(const QString& message);
    void onNewClientConnected(std::size_t id){
        logger_->logMessage("New client connected, id = " + QString::number(id));
    }

private:
    Ui::MainWindowClass     ui;
    std::shared_ptr<Logger> logger_;
    DeviceServer server_;
    DevicesTableModel*       devicesModel_ = nullptr;

//    TcpServer server_;
};
