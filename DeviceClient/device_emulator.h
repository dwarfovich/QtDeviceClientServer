#pragma once

#include <QTcpSocket>

class DeviceEmulator{
public:
private:
    QTcpSocket* socket_ = nullptr;
};