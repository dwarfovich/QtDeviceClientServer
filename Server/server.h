#pragma once

#include <QObject>
#include <QTcpServer>

#include <memory>

class Logger;
class ServerWorker;

class Server : public QObject
{
    Q_OBJECT

public:
    Server(QObject* parent, const std::shared_ptr<Logger>& logger);
    ~Server() { stop(); }

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

private:
    QThread       thread_;
    ServerWorker* worker_;
    std::shared_ptr<Logger> logger_;
};
