#pragma once

#include "Core/i_data_writer.h"

#include <QTcpSocket>

/*

NetworkMetrics: метрики сети (например, {"type": "NetworkMetrics", "bandwidth": 100.5, "latency": 12.3, "packet_loss": 0.01})
DeviceStatus: статус устройства (например, {"type": "DeviceStatus", "uptime": 3600, "cpu_usage": 25, "memory_usage": 60}).
- Log: текстовый лог (например, {"type": "Log", "message": "Interface eth0 restarted", "severity": "INFO"}).

*/

// DeviceMessage -> Json -> TCP
//                       -> File
//               -> Bin  -> TCP
//                       -> File

class BackendCommunicator : public QObject {
    Q_OBJECT
public:
    BackendCommunicator(QObject* parent) : QObject{parent}{}

    void setWriter(IDataWriter* writer){
        Q_ASSERT(writer);

        writer->setParent(this);
        writer_ = writer;
    }

    void sendString(const QString& text){
        qDebug() << "Sending string: " << text;
        writer_->write(text.toUtf8());
    }

public slots:
    private:
    IDataWriter* writer_;
};