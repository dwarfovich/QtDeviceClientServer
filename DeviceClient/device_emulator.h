#pragma once

#include "backend_communicator.h"
#include "device_emulator_settings.h"

#include <QTimer>
#include <QRandomGenerator>

#include <atomic>

class DeviceEmulator : public QObject
{
    Q_OBJECT

public:
    DeviceEmulator(QObject* parent) : QObject { parent }, communicator_ { new BackendCommunicator { this } }
    {
        // connect(communicator_, &BackendCommunicator::connected, this, &DeviceEmulator::onNetworkConnected);
        connect(communicator_, &BackendCommunicator::disconnected, this, &DeviceEmulator::onNetworkDisconnected);
        connect(&dataTimer_, &QTimer::timeout, this, &DeviceEmulator::onDataTimerTimeout);

        communicator_->setStartRequestHandler([this](const device_message::StartRequest& message) {
            onStartRequested(message);
        });
    }

public slots:
    void start() { communicator_->start(); }
    void stop()
    {
        dataTimer_.stop();
        communicator_->stop();
    }

private slots:
    void onNetworkDisconnected() { dataTimer_.stop(); }

    void onDataTimerTimeout() { sendRandomData(); }

    void sendRandomData()
    {
        using namespace device_message;
        switch (randomGenerator_.bounded(3)) {
            case 0:
                communicator_->sendMessage(NetworkMetrics { .bandwidth  = randomDouble(0., 1000.),
                                                            .latency    = randomDouble(0., 1000.),
                                                            .packetLoss = randomDouble(0., 100.) });
                break;
            case 1:
                communicator_->sendMessage(DeviceStatus { .uptime      = randomInt(0, 10),
                                                          .cpuUsage    = randomInt(0, 100),
                                                          .memoryUsage = randomInt(0, 100) });
                break;
            case 2:
                communicator_->sendMessage(Log { .message = randomText(), .severity = randomLogMessageSeverity() });
                break;
            default: Q_UNREACHABLE();
        }
    }

private:
    void onStartRequested(const device_message::StartRequest& message)
    {
        dataTimer_.start(randomDouble(settings_.minDataSendingPeriod, settings_.maxDataSendingPeriod));
    }

    double randomDouble(double min, double max)
    {
        Q_ASSERT(min < max);

        return min + randomGenerator_.generateDouble() * (max - min);
    }

    template<typename T>
    T randomInt(T min, T max)
    {
        Q_ASSERT(min < max);

        return randomGenerator_.bounded(min, max);
    }

    QString randomText() { return {}; }

    device_message::LogMessageSeverity randomLogMessageSeverity() { return {}; }

private:
    inline static constexpr DeviceEmulatorSettings settings_;

    QTimer               dataTimer_;
    BackendCommunicator* communicator_ = nullptr;
    QRandomGenerator     randomGenerator_;
};