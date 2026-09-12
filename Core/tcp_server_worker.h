#pragma once

#include "Core/logger.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

#include <limits>
#include <unordered_map>

class TcpServerWorker : public QObject {
    Q_OBJECT

public:
    TcpServerWorker(QObject* parent, const std::shared_ptr<Logger>& logger)
        : QObject{parent}, server_{new QTcpServer{this}}, logger_{logger}, abortTimer_{new QTimer{this}}
    {
        connect(server_, &QTcpServer::newConnection, this, &TcpServerWorker::onNewConnection);
    }

public slots:
    void start(quint16 port)
    {
        assertWorkerThread();

        if (stopPended_) {
            return;
        }

        if (!server_->listen(QHostAddress::Any, port)) {
            logger_->logMessage(QString("Failed to start server: %1").arg(server_->errorString()));
            return;
        }

        logger_->logMessage(QString("Server started on port %1").arg(port));
    }

    void stop()
    {
        assertWorkerThread();

        if (stopPended_) {
            return;
        }

        stopPended_ = true;
        server_->close();

        std::vector<QTcpSocket*> socketsBackup;
        socketsBackup.reserve(idToSocketsMap_.size());
        for (const auto& [_, socket] : idToSocketsMap_) {
            socketsBackup.push_back(socket);
        }
        for (auto* socket : socketsBackup) {
            socket->disconnectFromHost();
        }

        checkStopStatus();
    }

    void write(std::size_t id, const QByteArray& data)
    {
        assertWorkerThread();

        auto iter = idToSocketsMap_.find(id);
        if (iter != idToSocketsMap_.cend()) {
            iter->second->write(data);
        } else {
            logger_->logMessage("Couldn't find client with id " + QString::number(id) + " for writing data");
        }
    }

signals:
    void stopped();
    void newClientConnected(std::size_t id, QHostAddress address);
    void clientDisconnected(std::size_t id);
    void dataReceived(std::size_t clientId, QByteArray data);

private slots:
    void onNewConnection()
    {
        while (server_->hasPendingConnections() && !stopPended_) {
            QTcpSocket* socket = server_->nextPendingConnection();
            if (nextSocketId_ == std::numeric_limits<std::size_t>::max()) {
                socket->disconnectFromHost();
            } else {
                if (nextSocketId_ == maxClientId_) {
                    logger_->logMessage("Failed to acquire new client ID, stopping accepting new connections");
                    return;
                }
                const auto newId = nextSocketId_++;
                idToSocketsMap_.insert({newId, socket});
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
        auto iter = socketsToIdMap_.find(socket);
        if (iter != socketsToIdMap_.cend()) {
            const auto id = iter->second;
            emit dataReceived(id, data);
        } else {
            Q_ASSERT(false);
        }
    }

    void onDisconnected()
    {
        auto* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket) {
            return;
        }

        auto iter = socketsToIdMap_.find(socket);
        if (iter == socketsToIdMap_.cend()) {
            Q_ASSERT(false);
            return;
        }
        const auto id = iter->second;
        idToSocketsMap_.erase(id);
        socketsToIdMap_.erase(socket);
        socket->deleteLater();
        socket->disconnect(this);
        emit clientDisconnected(id);

        checkStopStatus();
    }

private:  // methods
    void assertWorkerThread() const
    {
        Q_ASSERT(QThread::currentThread() == thread());
    }

    void checkStopStatus()
    {
        if (!stopPended_) {
            return;
        }

        if (idToSocketsMap_.empty()) {
            abortTimer_->stop();
            emit stopped();
            return;
        }

        if (!abortTimer_->isActive()) {
            QTimer::singleShot(socketsAbortTimeout_, this, [this]() {
                for (auto& [id, socket] : idToSocketsMap_) {
                    socket->disconnect(this);
                    socket->abort();
                }

                idToSocketsMap_.clear();
                socketsToIdMap_.clear();

                emit stopped();
            });
        }
    }

private:  // data
    static constexpr std::size_t maxClientId_ = -1;
    static constexpr int socketsAbortTimeout_ = 30000;

    QTcpServer* server_;
    std::unordered_map<std::size_t, QTcpSocket*> idToSocketsMap_;
    std::unordered_map<QTcpSocket*, std::size_t> socketsToIdMap_;
    std::size_t nextSocketId_ = 1;
    std::shared_ptr<Logger> logger_ = nullptr;
    bool stopPended_ = false;
    QTimer* abortTimer_ = nullptr;
};