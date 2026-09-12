#pragma once

#include "Core/messages.h"

#include <QMetaProperty>
#include <QVariantMap>

template <typename T>
void addProperties(QVariantMap& target, const T& object)
{
    const QMetaObject& meta = T::staticMetaObject;
    for (int i = meta.propertyOffset(); i < meta.propertyCount(); ++i) {
        QMetaProperty property = meta.property(i);
        target[property.name()] = {};
    }
}

struct DeviceData {
    Q_GADGET

        public:
    DeviceData()
    {
        addProperties(properties_, device_message::NetworkMetrics{});
        addProperties(properties_, device_message::DeviceStatus{});
        addProperties(properties_, device_message::Log{});
    }

    std::size_t clientId_ = 0;
    QVariantMap properties_;
};
