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
        : QObject { parent }, tcpServer_ { new QTcpServer { this } }, logger_ { logger }
    {
        connect(tcpServer_, &QTcpServer::newConnection, this, &TcpServerWorker::onNewConnection);
    }

signals:
    void stopped();
    void newClientConnected(std::size_t id, QHostAddress address);
    void dataReceived(std::size_t clientId, QByteArray data);
    void clientDisconnected(std::size_t id);

public slots:
    void start(quint16 port)
    {
        if (!tcpServer_->listen(QHostAddress::Any, port)) {
            logger_->logMessage(QString("Failed to start server: %1").arg(tcpServer_->errorString()));
            return;
        }

        logger_->logMessage(QString("Server started on port %1").arg(port));
    }
    void stop()
    {
        tcpServer_->close();
        //for (auto* socket : clients_) {
        //    socket->disconnectFromHost();
        //}
        //clients_.clear();
        for (auto& [id, socket]: idToSocketsMap_){
            socket->disconnectFromHost();
        }
        idToSocketsMap_.clear();
        socketsToIdMap_.clear();

        emit stopped();
    }

    void onNewConnection()
    {
        while (tcpServer_->hasPendingConnections()) {
            QTcpSocket* socket = tcpServer_->nextPendingConnection();
            if (nextSocketId_ == std::numeric_limits<std::size_t>::max()) {
                socket->disconnectFromHost();
            } else {
                const auto newId = nextSocketId_++;
                idToSocketsMap_.insert({ newId, socket });
                socketsToIdMap_.insert({socket, newId});
                connect(socket, &QTcpSocket::disconnected, this, &TcpServerWorker::onDisconnected);
                connect(socket, &QTcpSocket::readyRead, this, &TcpServerWorker::onReadyRead);
                emit newClientConnected(newId, socket->peerAddress());
            }
        }
    }

    void onReadyRead()
    {
        auto* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket) {
            return;
        }

        const auto& data = socket->readAll();
        const auto  id   = socketsToIdMap_.at(socket);
        emit dataReceived(id, data);
        //qDebug() << "Received:" << data;
        //socket->write("Hello from server!\n");
    }

    void write(std::size_t id, const QByteArray& data)
    {
        auto iter =idToSocketsMap_.find(id);
        if (iter != idToSocketsMap_.cend()){
            iter->second->write(data);
        }
    }

    void onDisconnected()
    {
        auto* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket) {
            return;
        }

        qDebug() << "Client disconnected";

        const auto id = socketsToIdMap_.at(socket);
        idToSocketsMap_.erase(id);
        socketsToIdMap_.erase(socket);
        socket->deleteLater();
        emit clientDisconnected(id);
    }

private:
    QTcpServer*                                  tcpServer_;
    std::unordered_map<std::size_t, QTcpSocket*> idToSocketsMap_;
    std::unordered_map<QTcpSocket*, std::size_t> socketsToIdMap_;
    std::size_t                                  nextSocketId_ = 1;
    std::shared_ptr<Logger>                      logger_       = nullptr;
};