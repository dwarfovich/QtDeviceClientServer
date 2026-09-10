#include "tcp_server.h"
#include "tcp_server_worker.h"
#include "logger.h"

#include <QDebug>
#include <QTcpSocket>

TcpServer::TcpServer(QObject* parent, const std::shared_ptr<Logger>& logger)
    : QObject { parent }, logger_ { logger }, worker_ { new TcpServerWorker { this, logger } }
{
    connect(worker_, &TcpServerWorker::newClientConnected, this, &TcpServer::newClientConnected, Qt::QueuedConnection);
    connect(worker_, &TcpServerWorker::dataRead, this, &TcpServer::dataReceived, Qt::QueuedConnection);
    connect(worker_, &TcpServerWorker::clientDisconnected, this, &TcpServer::clientDisconnected, Qt::QueuedConnection);
    connect(this, &TcpServer::stopWorker, worker_, &TcpServerWorker::stop, Qt::QueuedConnection);
    connect(worker_, &TcpServerWorker::stopped, &thread_, &QThread::quit, Qt::DirectConnection);
    connect(&thread_, &QThread::finished, worker_, &QObject::deleteLater);
}

void TcpServer::start(quint16 port)
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