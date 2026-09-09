#pragma once

#include "Core/logger.h"

#include <QTcpServer>
#include <QTcpSocket>

class ServerWorker : public QObject
{
    Q_OBJECT

public:
    ServerWorker(QObject* parent, const std::shared_ptr<Logger>& logger)
        : QObject { parent }, server_ { new QTcpServer { this } }, logger_ { logger }
    {
        connect(server_, &QTcpServer::newConnection, this, &ServerWorker::onNewConnection);
    }

signals:
    void stopped();

public slots:
    void start(quint16 port)
    {
        if (!server_->listen(QHostAddress::Any, port)) {
            logger_->logMessage(QString("Failed to start server: %1").arg(server_->errorString()));
            return;
        }

        logger_->logMessage(QString("Server started on port %1").arg(port));
    }
    void stop()
    {
        server_->close();
        for (auto* socket : clients_) {
            socket->disconnectFromHost();
        }
        clients_.clear();

        emit stopped();
    }

    void onNewConnection()
    {
        while (server_->hasPendingConnections()) {
            QTcpSocket* socket = server_->nextPendingConnection();
            clients_.push_back(socket);

            logger_->logMessage("Client connected to server: " + socket->peerAddress().toString() + " "
                                + QString::number(socket->peerPort()));

            // connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
            connect(socket, &QTcpSocket::disconnected, this, &ServerWorker::onDisconnected);
        }
    }

    void onReadyRead()
    {
        auto* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket) {
            return;
        }

        const QByteArray data = socket->readAll();
        qDebug() << "Received:" << data;
        socket->write("Hello from server!\n");
    }

    void onDisconnected()
    {
        auto* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket) {
            return;
        }

        qDebug() << "Client disconnected";

        std::erase(clients_, socket);
        socket->deleteLater();
    }

private:
    QTcpServer*              server_;
    std::vector<QTcpSocket*> clients_;
    std::shared_ptr<Logger>  logger_;
};