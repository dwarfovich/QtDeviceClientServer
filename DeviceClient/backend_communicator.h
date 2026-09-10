#pragma once

#include "Core/i_data_writer.h"

#include <QTcpSocket>

class BackendCommunicator : public QObject
{
    Q_OBJECT
public:
    BackendCommunicator(QObject* parent) : QObject { parent } {}

    void setWriter(IDataWriter* writer)
    {
        Q_ASSERT(writer);

        writer->setParent(this);
        writer_ = writer;
    }

    void sendString(const QString& text)
    {
        qDebug() << "Sending string: " << text;
        writer_->write(text.toUtf8());
    }

public slots:
private:
    IDataWriter* writer_;
};