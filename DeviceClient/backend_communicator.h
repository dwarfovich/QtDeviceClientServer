#pragma once

#include "Core/i_data_writer.h"
#include "Core/tcp_data_writer.h"
#include "Core/messages.h"

#include <QTcpSocket>
#include< QTimer>

class BackendCommunicator : public QObject
{
    Q_OBJECT
public:
    BackendCommunicator(QObject* parent) : QObject { parent } //, writer_ { writer }
    {
        //Q_ASSERT(writer);

        connectionTimer_.setInterval(5000);

        connect(&connectionTimer_, &QTimer::timeout, this, &BackendCommunicator::onDisconnected);
        connect(&socket_, &QTcpSocket::connected, this, &BackendCommunicator::onConnected);
        // writer_->setParent(this);
    }

    void start() { 
        connectSocket();
        connectionTimer_.start(); }

    void sendString(const QString& text)
    {
        qDebug() << "Sending string: " << text;
        // writer_->write(text.toUtf8());
        socket_.write(text.toUtf8() + device_message::dataEndMarker);
    }

    template <typename MessageType>
    void sendMessage(const MessageType& message)
    {

    }

public slots:
signals:
    void disconnected();
    void connected();

private slots:
    void onDisconnected() { connectionTimer_.start();
        emit disconnected();
    }
    void onConnected() { connectionTimer_.stop();
        emit connected();
    }

private:
    void connectSocket()
    {
        if (socket_.state() != QAbstractSocket::UnconnectedState) {
            return;
        }
        socket_.connectToHost(QHostAddress::LocalHost, 12345);
    }

private:
    QTimer     connectionTimer_;
    QTcpSocket socket_;
    // TcpDataWriter* writer_;
};