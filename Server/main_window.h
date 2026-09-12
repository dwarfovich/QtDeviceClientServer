#pragma once

#include "device_server_controller.h"

#include "Core/logger.h"

#include <QGroupbox>
#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>
#include <QCheckBox>

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

        auto* startAllDevicesButton = new QPushButton{"Start all devices", centralWidget};
        auto* stopAllDevicesButton = new QPushButton{"Stop all devices", centralWidget};
        connect(startAllDevicesButton, &QPushButton::clicked, this, &MainWindow::onStartAllDevicesButtonClicked);
        connect(stopAllDevicesButton, &QPushButton::clicked, this, &MainWindow::onStopAllDevicesButtonClicked);

        buttonsLayout->addWidget(startAllDevicesButton);
        buttonsLayout->addWidget(stopAllDevicesButton);
        buttonsLayout->addStretch();

        mainLayout->addLayout(buttonsLayout);

        auto* contentLayout = new QHBoxLayout();

        devicesTable_ = new QTableView{centralWidget};
        devicesTable_->setModel(serverController_.devicesModel());
        
        controlBox_ = new QGroupBox{"Controls", centralWidget};
        controlBox_->setDisabled(true);
        controlBox_->setMinimumWidth(300);

        auto* controlsLayout = new QVBoxLayout{controlBox_};

        controlsLayout->addStretch();

        auto* signalLayout = new QHBoxLayout;
        signalLampCheckBox = new QCheckBox{"Signal lamp state"};
        signalLayout->addWidget(signalLampCheckBox);
        signalLayout->addStretch();

        controlsLayout->addLayout(signalLayout);

        auto* controlButtonsLayout = new QHBoxLayout;
        startDeviceButton = new QPushButton{"Start device"};
        stopDeviceButton = new QPushButton{"Stop device"};

        controlButtonsLayout->addWidget(startDeviceButton);
        controlButtonsLayout->addWidget(stopDeviceButton);
        controlButtonsLayout->addStretch();

        controlsLayout->addLayout(controlButtonsLayout);

        controlsLayout->addStretch();

        connect(startDeviceButton, &QPushButton::clicked, this, &MainWindow::onStartDeviceButtonClicked);
        connect(stopDeviceButton, &QPushButton::clicked, this, &MainWindow::onStopDeviceButtonClicked);

        contentLayout->addWidget(devicesTable_, 1);
        contentLayout->addWidget(controlBox_);

        mainLayout->addLayout(contentLayout);

        dataTable_ = new QTableView{centralWidget};
        dataTable_->setModel(serverController_.dataModel());
        mainLayout->addWidget(dataTable_);

        logText = new QPlainTextEdit{centralWidget};
        logText->setReadOnly(true);
        mainLayout->addWidget(logText);

        connect(devicesTable_->selectionModel(),
                &QItemSelectionModel::selectionChanged,
                this,
                &MainWindow::onDeviceSelected);
        connect(dataTable_->selectionModel(),
                &QItemSelectionModel::selectionChanged,
                this,
                &MainWindow::onDeviceSelected);

        setupWindowGeometry();
    }

private slots:
    void onNewLogMessage(const QString& message)
    {
        logText->appendPlainText(message);
    }

    void onDeviceSelected(const QItemSelection& selected, const QItemSelection&)
    {
        if (selected.isEmpty()) {
            controlBox_->setDisabled(true);
            controlBox_->setTitle("Selected device: none");
            signalLampCheckBox->setDisabled(true);
            selectedDeviceId_ = 0;
            return;
        }

        const QModelIndex index = selected.indexes().first();
        const auto* model = index.model();

        int column = 0;
        if (model == devicesTable_->model()) {
            column = DevicesTableModel::idColumn();
        } else if (model == dataTable_->model()) {
            column = DataTableModel::idColumn();
        }
        const std::size_t id = index.siblingAtColumn(column).data().toULongLong();
        selectedDeviceId_ = id;
        controlBox_->setEnabled(true);
        controlBox_->setTitle("Selected device: " + QString::number(id));
        signalLampCheckBox->setEnabled(true);
        const bool newState = serverController_.dataModel()->signalLampState(id);
        signalLampCheckBox->setChecked(newState);
    }

    void onSignalStateCheckBoxChanged(bool toggled){
        if (selectedDeviceId_ != 0){
        serverController_.enableDeviceSignalLamp(selectedDeviceId_, toggled);
        }
    }

    void onStartDeviceButtonClicked(){
        if (selectedDeviceId_ != 0) {
            serverController_.startDevice(selectedDeviceId_);
        }
    }
    void onStopDeviceButtonClicked() {
        if (selectedDeviceId_ != 0) {
            serverController_.stopoDevice(selectedDeviceId_);
        }
    }

    void onStartAllDevicesButtonClicked()
    {
        serverController_.startAllDevices();
    }

    void onStopAllDevicesButtonClicked()
    {
        serverController_.stopAllDevices();
    }


    //connect(startDeviceButton, &QPushButton::clicked, this, &MainWindow::onStartDeviceButtonClicked);
    //connect(stopDeviceButton, &QPushButton::clicked, this, &MainWindow::onStopDeviceButtonClicked);


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
    QGroupBox* controlBox_ = nullptr;
    QCheckBox* signalLampCheckBox = nullptr;
    QPushButton* startDeviceButton = nullptr;
    QPushButton* stopDeviceButton = nullptr;
    std::size_t selectedDeviceId_ = 0;
};
