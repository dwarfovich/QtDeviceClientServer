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
    TcpServer(QObject* parent, const std::shared_ptr<Logger>& logger);
    ~TcpServer() { stop(); }

    void sendMessage(std::size_t clientId, const QByteArray& data){
        auto* w = worker_;
        QMetaObject::invokeMethod(
            w,
            [w,clientId, data]() {
                w->write(clientId, data);
            },
            Qt::QueuedConnection);
    }
public slots:
    void start(quint16 port);
    void stop()
    {
        qDebug() << "Server stop";
        if (!thread_.isRunning()){
            return;
        }
        emit stopWorker();
        thread_.wait();
    }


signals:
    void stopWorker();
    void startWorker(qint16 port);
    void newClientConnected(std::size_t id, const QHostAddress& address);
    void clientDisconnected(std::size_t id);
    void dataReceived(std::size_t clientId, QByteArray data);
    //void messageReceived(std::size_t clientId, )
private:
    QThread       thread_;
    TcpServerWorker* worker_;
    std::shared_ptr<Logger> logger_;
};
