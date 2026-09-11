#pragma once

#include "backend_communicator.h"
#include "device_emulator_settings.h"
#include "text_generator.h"

#include <QRandomGenerator>
#include <QTimer>

#include <atomic>

class DeviceEmulator : public QObject {
    Q_OBJECT

public:
    DeviceEmulator(QObject* parent) : QObject{parent}, communicator_{new BackendCommunicator{this}}
    {
        // connect(communicator_, &BackendCommunicator::connected, this, &DeviceEmulator::onNetworkConnected);
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
        switch (randomGenerator_.bounded(3)) {
            case 0:
                communicator_->sendMessage(NetworkMetrics{.bandwidth = randomDouble2Precision(0., 1000.),
                                                          .latency = randomDouble2Precision(0., 1000.),
                                                          .packetLoss = randomDouble2Precision(0., 100.)});
                break;
            case 1:
                communicator_->sendMessage(DeviceStatus{.uptime = randomInt(0, 10),
                                                        .cpuUsage = randomInt(0, 100),
                                                        .memoryUsage = randomInt(0, 100)});
                break;
            case 2:
                communicator_->sendMessage(Log{.message = randomText(), .severity = randomLogMessageSeverity()});
                break;
            default:
                Q_UNREACHABLE();
        }
    }

private:
    double randomDouble2Precision(double min, double max)
    {
        Q_ASSERT(min < max);

        return std::round((min + randomGenerator_.generateDouble() * (max - min)) * 100.0) / 100.0;
    }

    template <typename T>
    T randomInt(T min, T max)
    {
        Q_ASSERT(min < max);

        return randomGenerator_.bounded(min, max);
    }

    QString randomText()
    {
        return generateText(randomInt(0, 3));
    }

    device_message::LogMessageSeverity randomLogMessageSeverity()
    {
        return {};
    }

private:
    inline static constexpr DeviceEmulatorSettings settings_;

    QTimer dataTimer_;
    BackendCommunicator* communicator_ = nullptr;
    QRandomGenerator randomGenerator_;
};