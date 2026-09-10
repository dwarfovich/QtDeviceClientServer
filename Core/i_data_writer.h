#pragma once

#include< QObject>

class IDataWriter: public QObject
{
    Q_OBJECT

public:
    IDataWriter(QObject* parent) : QObject{parent}{}
    virtual ~IDataWriter() = default;

    virtual void write(const QByteArray& data) = 0;
};