#pragma once

#include "ui_main_window.h"
#include "server.h"

#include "Core/logger.h"

#include <QtWidgets/QMainWindow>

#include <memory>

class Server;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const std::shared_ptr<Logger>& logger, QWidget* parent = nullptr);
    ~MainWindow(){
        //server_.stop();
    }

private slots:
    void onNewLogMessage(const QString& message);

private:
    Ui::MainWindowClass     ui;
    std::shared_ptr<Logger> logger_;
    Server server_;
};
