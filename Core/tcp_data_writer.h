#pragma once

#include "i_data_writer.h"
#include "message_end_marker.h"

#include <QObject>
#include <QTcpSocket>

class TcpDataWriter : public IDataWriter
{
    Q_OBJECT
public:
    TcpDataWriter(QObject* parent) : IDataWriter { parent }, socket_ { new QTcpSocket { this } }
    {
        connect(socket_, &QTcpSocket::disconnected, this, &TcpDataWriter::networkDisconnected);
        connect(socket_, &QTcpSocket::connected, this, &TcpDataWriter::networkConnected);
    }

    void write(const QByteArray& data) override
    {
        socket_->write(data);
        socket_->write(device_message::dataEndMarker);
    };

    void connectTo(const QHostAddress& address, quint16 port)
    {
        qDebug() << address;
        socket_->connectToHost(address, port);
    }

public slots:
    void disconnect() { socket_->disconnectFromHost(); }

signals:
    void networkDisconnected();
    void networkConnected();
    void messageReceived(const QByteArray& messageData);

private:
    QTcpSocket* socket_ = nullptr;
};