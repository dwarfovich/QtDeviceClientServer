#pragma once

#include "tcp_server_worker.h"

#include <QObject>
#include <QTcpServer>
#include <QThread>

#include <memory>

class Logger;
class TcpServerWorker;

class TcpServer : public QObject
{
    Q_OBJECT

public:
    TcpServer(QObject* parent, const std::shared_ptr<Logger>& logger)
        : QObject { parent }, logger_ { logger }, worker_ { new TcpServerWorker { this, logger } }
    {
        connect(
            worker_, &TcpServerWorker::newClientConnected, this, &TcpServer::newClientConnected, Qt::QueuedConnection);
        connect(worker_, &TcpServerWorker::dataReceived, this, &TcpServer::dataReceived, Qt::QueuedConnection);
        connect(
            worker_, &TcpServerWorker::clientDisconnected, this, &TcpServer::clientDisconnected, Qt::QueuedConnection);
        connect(this, &TcpServer::stopWorker, worker_, &TcpServerWorker::stop, Qt::QueuedConnection);
        connect(worker_, &TcpServerWorker::stopped, &thread_, &QThread::quit, Qt::DirectConnection);
        connect(&thread_, &QThread::finished, worker_, &QObject::deleteLater);
    }

    ~TcpServer() { stop(); }

    void sendMessage(std::size_t clientId, const QByteArray& data)
    {
        auto* worker = worker_;
        QMetaObject::invokeMethod(
            worker,
            [worker, clientId, data]() {
                worker->write(clientId, data);
            },
            Qt::QueuedConnection);
    }

public slots:
    void start(quint16 port)
    {
        if (thread_.isRunning()) {
            return;
        }
        worker_->setParent(nullptr);
        worker_->moveToThread(&thread_);
        connect(
            &thread_,
            &QThread::started,
            worker_,
            [this, port] {
                worker_->start(port);
            },
            Qt::SingleShotConnection);

        thread_.start();
    }

    void stop()
    {
        if (!thread_.isRunning()) {
            return;
        }
        emit stopWorker();
        thread_.wait();
    }

signals:
    void newClientConnected(std::size_t id, const QHostAddress& address);
    void clientDisconnected(std::size_t id);
    void dataReceived(std::size_t clientId, QByteArray data);

protected:
    Q_SIGNAL void stopWorker();
    Q_SIGNAL void startWorker(qint16 port);

private:
    QThread                 thread_;
    TcpServerWorker*        worker_;
    std::shared_ptr<Logger> logger_;
};
