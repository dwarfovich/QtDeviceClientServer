#pragma once

#include "backend_communicator.h"
#include "device_emulator_settings.h"

#include "Core/text_generator.h"

#include <QRandomGenerator>
#include <QTimer>

class DeviceEmulator : public QObject {
    Q_OBJECT

public:
    DeviceEmulator(QObject* parent) : QObject{parent}, communicator_{new BackendCommunicator{this}}
    {
        connect(communicator_, &BackendCommunicator::disconnected, this, &DeviceEmulator::onNetworkDisconnected);
        connect(communicator_, &BackendCommunicator::newMessageReceived, this, &DeviceEmulator::processMessage);
        connect(&dataTimer_, &QTimer::timeout, this, &DeviceEmulator::onDataTimerTimeout);
    }

public slots:
    void start()
    {
        communicator_->start();
    }

    void stop()
    {
        dataTimer_.stop();
        communicator_->stop();
    }

private slots:
    void processMessage(const device_message::Message& message)
    {
        if (message.type() == device_message::Type::StartRequest) {
            dataTimer_.start(randomDouble2Precision(settings_.minDataSendingPeriod, settings_.maxDataSendingPeriod));
            qDebug() << "Received StartRequest message, starting device emulation";
        } else if (message.type() == device_message::Type::DeviceCommand) {
            executeCommand(as<device_message::DeviceCommandMessage>(message));
        } else {
            qDebug() << "Received unsupported message of type: " << device_message::toString(message.type());
        }
    }

    void onNetworkDisconnected()
    {
        dataTimer_.stop();
    }

    void onDataTimerTimeout()
    {
        sendRandomData();
    }

    void sendRandomData()
    {
        using namespace device_message;
        switch (QRandomGenerator::global()->bounded(3)) {
            case 0:
                communicator_->sendMessage(NetworkMetrics{.bandwidth = randomDouble2Precision(0., 1000.),
                                                          .latency = randomDouble2Precision(0., 1000.),
                                                          .packetLoss = randomDouble2Precision(0., 100.)});
                break;
            case 1:
                communicator_->sendMessage(DeviceStatus{.uptime = randomInt(0, 10),
                                                        .cpuUsage = randomInt(0, 100),
                                                        .signalLampState = signalLampState_,
                                                        .isActive = dataTimer_.isActive(),
                                                        .memoryUsage = randomDouble2Precision(0, 100)});
                break;
            case 2:
                communicator_->sendMessage(Log{.message = randomText(), .severity = randomLogMessageSeverity()});
                break;
            default:
                break;
        }
    }

private:
    void executeCommand(const device_message::DeviceCommandMessage& message)
    {
        if (message.command == DeviceCommands::EnableSignalLamp) {
            changeSignalLampState(message.parameter);
        } else if (message.command == DeviceCommands::StartStop) {
            changeWorkState(message.parameter);
        } else {
            qDebug() << "Received unknown command, skipping it";
        }
    }

    void changeWorkState(const QVariant& parameter)
    {
        bool newState = parameter.toBool();
        if (newState) {
            if (!dataTimer_.isActive()) {
                dataTimer_.start();
            }
        } else {
            dataTimer_.stop();
            communicator_->sendMessage(device_message::DeviceStatus{.uptime = randomInt(0, 10),
                                                                    .cpuUsage = randomInt(0, 100),
                                                                    .signalLampState = signalLampState_,
                                                                    .isActive = dataTimer_.isActive(),
                                                                    .memoryUsage = randomDouble2Precision(0, 100)});
        }
    }

    void changeSignalLampState(const QVariant& parameter)
    {
        signalLampState_ = parameter.toBool();
    }

private:
    inline static constexpr DeviceEmulatorSettings settings_;

    bool signalLampState_ = false;

    QTimer dataTimer_;
    BackendCommunicator* communicator_ = nullptr;
};