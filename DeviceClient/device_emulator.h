#pragma once

#include "backend_communicator.h"

#include "Core/tcp_data_writer.h"

#include <QTcpSocket>
#include <QTimer>
#include <QThread>

#include <atomic>

class DeviceEmulator : public QObject
{
    Q_OBJECT

public:
    DeviceEmulator(QObject* parent)
        : QObject { parent }
        , communicator_ { new BackendCommunicator { this } }
        //, connectionTimer_ { new QTimer { this } }
        , dataTimer_ { new QTimer { this } }
    {
        connect(communicator_, &BackendCommunicator::connected, this, &DeviceEmulator::onNetworkConnected);
        connect(communicator_, &BackendCommunicator::disconnected, this, &DeviceEmulator::onNetworkDisconnected);

        //writer_ = new TcpDataWriter(this);
        //communicator_->setWriter(writer_);

        //connect(writer_, &TcpDataWriter::networkDisconnected, this, &DeviceEmulator::onNetworkDisconnected);
        //connect(writer_, &TcpDataWriter::networkConnected, this, &DeviceEmulator::onNetworkConnected);
        //connect(writer_, &TcpDataWriter::messageReceived, this, &DeviceEmulator::onBackendMessageReceived);

        //connect(this, &DeviceEmulator::stopRequested, writer_, &TcpDataWriter::disconnect);

        //connect(connectionTimer_, &QTimer::timeout, this, &DeviceEmulator::tryConnect);
        //connect(dataTimer_, &QTimer::timeout, this, &DeviceEmulator::sendData);

        //connectionTimer_->setInterval(100);
        //dataTimer_->setInterval(500);
    }

public slots:
    void start() { 
        communicator_->start();
        //tryConnect();
        //connectionTimer_->start();
    }
    void stop()
    {
        //connectionTimer_->stop();
        //dataTimer_->stop();
        //emit stopRequested();
    }

protected:
signals:
    void stopRequested();

private slots:
    void onNetworkConnected()
    {
       // connectionTimer_->stop();
        dataTimer_->start(600);
    }
    //void tryConnect() { writer_->connectTo(QHostAddress::LocalHost, 12345); }
    void onNetworkDisconnected()
    {
        dataTimer_->stop();
      //  connectionTimer_->start();
    }
    void onBackendMessageReceived(const QByteArray& data) {
        qDebug() << "Client received message: " << data;
    }
    void sendData()
    {
        communicator_->sendString("Hello from device!");
        dataTimer_->setInterval(1000);
    }

private:
    bool connectToBackend(const QHostAddress& address, quint16 port) {}

private:
    static constexpr int connectionPeriod_ = 5000;

    std::atomic_flag     stopRequested_;
    //QTimer*              connectionTimer_ = nullptr;
    QTimer*              dataTimer_       = nullptr;
    //TcpDataWriter*       writer_          = nullptr;
    BackendCommunicator* communicator_    = nullptr;
};