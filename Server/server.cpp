#include "server.h"
#include "server_worker.h"

#include "Core/logger.h"

#include <QDebug>
#include <QTcpSocket>

Server::Server(QObject* parent, const std::shared_ptr<Logger>& logger)
    : QObject { parent },logger_{logger}, worker_ { new ServerWorker { this, logger } }
{
    connect(this, &Server::stopWorker, worker_, &ServerWorker::stop, Qt::QueuedConnection);
    connect(worker_, &ServerWorker::stopped, &thread_, &QThread::quit, Qt::DirectConnection);
    connect(&thread_, &QThread::finished, worker_, &QObject::deleteLater);
}

void Server::start(quint16 port)
{
    if (thread_.isRunning()){
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