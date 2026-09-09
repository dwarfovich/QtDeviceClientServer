#pragma once

#include "Core/logger.h"

#include <QTcpServer>
#include <QTcpSocket>

#include <unordered_map>

class TcpServerWorker : public QObject
{
    Q_OBJECT

public:
    TcpServerWorker(QObject* parent, const std::shared_ptr<Logger>& logger)
        : QObject { parent }, server_ { new QTcpServer { this } }, logger_ { logger }
    {
        connect(server_, &QTcpServer::newConnection, this, &TcpServerWorker::onNewConnection);
    }

signals:
    void stopped();
    void newClientConnected(std::size_t id, QHostAddress address);

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
            if (nextSocketId_ == std::numeric_limits<std::size_t>::max()) {
                socket->disconnectFromHost();
            } else {
                const auto nextId = nextSocketId_++;
                sockets_.insert({ nextId, socket });
                connect(socket, &QTcpSocket::disconnected, this, &TcpServerWorker::onDisconnected);
                // connect(socket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);
                emit newClientConnected(nextId, socket->peerAddress());

                logger_->logMessage("Client connected to server: " + socket->peerAddress().toString() + " "
                                    + QString::number(socket->peerPort()));
            }
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

    void write()
    {
        // clients_.front()->write
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
    QTcpServer*                                  server_;
    std::vector<QTcpSocket*>                     clients_;
    std::unordered_map<std::size_t, QTcpSocket*> sockets_;
    std::size_t                                  nextSocketId_ = 0;
    std::shared_ptr<Logger>                      logger_       = nullptr;
};