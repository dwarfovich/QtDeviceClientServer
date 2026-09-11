#pragma once

#include "tcp_server_worker.h"

#include <QObject>
#include <QTcpServer>
#include <QThread>

#include <memory>

class TcpServer : public QObject {
    Q_OBJECT

public:
    TcpServer(QObject* parent, const std::shared_ptr<Logger>& logger)
        : QObject{parent}, logger_{logger}, worker_{new TcpServerWorker{this, logger}} {
        connect(worker_,
                &TcpServerWorker::newClientConnected,
                this,
                &TcpServer::newClientConnected,
                Qt::QueuedConnection);
        connect(worker_, &TcpServerWorker::dataReceived, this, &TcpServer::dataReceived, Qt::QueuedConnection);
        connect(worker_,
                &TcpServerWorker::clientDisconnected,
                this,
                &TcpServer::clientDisconnected,
                Qt::QueuedConnection);
        connect(worker_, &TcpServerWorker::stopped, &workerThread_, &QThread::quit, Qt::DirectConnection);
        connect(&workerThread_, &QThread::finished, worker_, &QObject::deleteLater);
    }

    ~TcpServer() {
        stop();
    }

    void sendMessage(std::size_t clientId, const QByteArray& data) {
        auto* worker = worker_;
        QMetaObject::invokeMethod(
            worker,
            [worker, clientId, data]() { worker->write(clientId, data); },
            Qt::QueuedConnection);
    }

public slots:
    void start(quint16 port) {
        if (workerThread_.isRunning() || stopPended_) {
            return;
        }
        worker_->setParent(nullptr);
        worker_->moveToThread(&workerThread_);
        connect(
            &workerThread_,
            &QThread::started,
            worker_,
            [this, port] { worker_->start(port); },
            Qt::SingleShotConnection);

        workerThread_.start();
    }

    void stop() {
        Q_ASSERT(QThread::currentThread() != &workerThread_);

        if (!workerThread_.isRunning() || stopPended_) {
            return;
        }

        stopPended_ = true;
        auto* worker = worker_;
        QMetaObject::invokeMethod(worker, [worker]() { worker->stop(); }, Qt::QueuedConnection);
        workerThread_.wait();
    }

signals:
    void newClientConnected(std::size_t id, const QHostAddress& address);
    void clientDisconnected(std::size_t id);
    void dataReceived(std::size_t clientId, QByteArray data);

private:
    QThread workerThread_;
    TcpServerWorker* worker_ = nullptr;
    std::shared_ptr<Logger> logger_ = nullptr;
    bool stopPended_ = false;
};
