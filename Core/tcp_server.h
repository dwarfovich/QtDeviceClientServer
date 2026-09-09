#pragma once

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
    void dataReceived(std::size_t clientId, QByteArray data);
    //void messageReceived(std::size_t clientId, )
private:
    QThread       thread_;
    TcpServerWorker* worker_;
    std::shared_ptr<Logger> logger_;
};
