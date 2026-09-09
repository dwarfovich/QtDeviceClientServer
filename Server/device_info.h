#pragma once

#include "device_status.h"

#include <QHostAddress>

struct DeviceInfo{
    std::size_t id = 0;
    QHostAddress address;
    DeviceStatus status;
};