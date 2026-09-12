#pragma once

#include "device_server_controller.h"

#include "Core/logger.h"

#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

#include <memory>

class DevicesTableModel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(const std::shared_ptr<Logger>& logger, QWidget* parent)
        : QMainWindow{parent}, logger_{logger}, serverController_{this, logger}
    {
        Q_ASSERT(logger);

        connect(logger_.get(), &Logger::messageAdded, this, &MainWindow::onNewLogMessage);

        auto* centralWidget = new QWidget{this};
        setCentralWidget(centralWidget);

        auto* mainLayout = new QVBoxLayout{centralWidget};
        auto* buttonsLayout = new QHBoxLayout();

        auto* addButton = new QPushButton{"Add", centralWidget};
        auto* removeButton = new QPushButton{"Remove", centralWidget};
        auto* spacer = new QSpacerItem{0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum};
        buttonsLayout->addItem(spacer);

        buttonsLayout->addWidget(addButton);
        buttonsLayout->addWidget(removeButton);

        mainLayout->addLayout(buttonsLayout);
        devicesTable_ = new QTableView{centralWidget};
        devicesTable_->setModel(serverController_.devicesModel());
        mainLayout->addWidget(devicesTable_);
        dataTable_ = new QTableView{centralWidget};
        dataTable_->setModel(serverController_.dataModel());
        mainLayout->addWidget(dataTable_);
        logText = new QPlainTextEdit{centralWidget};
        logText->setReadOnly(true);
        mainLayout->addWidget(logText);

        setupWindowGeometry();
    }

private slots:
    void onNewLogMessage(const QString& message)
    {
        logText->appendPlainText(message);
    }

private:  // methods
    void setupWindowGeometry()
    {
        const QRect& geometry = QGuiApplication::primaryScreen()->availableGeometry();
        const int width = geometry.width() * 0.7;
        const int height = geometry.height() * 0.6;
        resize(width, height);
        move(geometry.x() + (geometry.width() - width) / 2, geometry.y() + (geometry.height() - height) / 2);
    }

private:  // data
    std::shared_ptr<Logger> logger_ = nullptr;
    DeviceServerController serverController_;
    QTableView* devicesTable_ = nullptr;
    QTableView* dataTable_ = nullptr;
    QPlainTextEdit* logText = nullptr;
};
