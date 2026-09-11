#pragma once

#include "Core/messages.h"
#include "Core/message_end_marker.h"
#include "Core/json_message_serializer.h"
#include "Core/json_message_deserializer.h"
#include "Core/default_network_parameters.h"

#include <QTcpSocket>
#include <QTimer>

class BackendCommunicator : public QObject
{
    Q_OBJECT
public:
    BackendCommunicator(QObject* parent) : QObject { parent }
    {
        connectionTimer_.setInterval(5000);

        connect(&connectionTimer_, &QTimer::timeout, this, &BackendCommunicator::onDisconnected);
        connect(&socket_, &QTcpSocket::connected, this, &BackendCommunicator::onConnected);
        connect(&socket_, &QTcpSocket::readyRead, this, &BackendCommunicator::onDataReceived);
    }

    void start()
    {
        connectSocket();
        connectionTimer_.start();
    }

    void stop()
    {
        connectionTimer_.stop();
        if (socket_.state() != QAbstractSocket::UnconnectedState) {
            socket_.disconnectFromHost();
            if (socket_.state() != QAbstractSocket::UnconnectedState) {
                socket_.abort();
            }
        }
    }

    void sendString(const QString& text) { socket_.write(text.toUtf8() + device_message::dataEndMarker); }

    template<typename MessageType>
    void sendMessage(const MessageType& message)
    {
        auto data = jsonSerializer_.serialize(message) + device_message::dataEndMarker;
        qDebug() << "Sending message: " << data;
        socket_.write(data);
    }

signals:
    void newMessageReceived(const device_message::Message& message);
    void disconnected();
    void connected();

private slots:
    void onDisconnected()
    {
        connectionTimer_.start();
        qDebug() << "Device disconnected, trying to reconnect";
        emit disconnected();
    }

    void onConnected()
    {
        connectionTimer_.stop();
        emit connected();
    }

    void onDataReceived()
    {
        currentData_ += socket_.readAll();
        if (currentData_.endsWith(device_message::dataEndMarker)) {
            currentData_.resize(currentData_.length() - device_message::dataEndMarker.length());
            const auto& message = jsonDeserializer_.deserialize(currentData_);
            emit newMessageReceived(message);
            currentData_.clear();
        }
    }

private:
    void connectSocket()
    {
        if (socket_.state() != QAbstractSocket::UnconnectedState) {
            return;
        }
        using default_network_parameters::serverAddress;
        using default_network_parameters::serverPort;
        socket_.connectToHost(serverAddress, serverPort);
    }

private:
    JsonMessageSerializer   jsonSerializer_;
    JsonMessageDeserializer jsonDeserializer_;
    QTimer                  connectionTimer_;
    QTcpSocket              socket_;
    QByteArray              currentData_;
};