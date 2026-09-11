#pragma once

#include "device_info.h"

#include <QAbstractTableModel>

#include <vector>

class DevicesTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    DevicesTableModel(QObject* parent = nullptr) : QAbstractTableModel{parent} {
    }

    int rowCount(const QModelIndex& parent = {}) const override {
        return devices_.size();
    }
    int columnCount(const QModelIndex& parent = {}) const override {
        return static_cast<int>(Columns::ColumnsCount);
    }
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(devices_.size())) {
            return {};
        }

        if (role != Qt::DisplayRole) {
            return {};
        }

        const auto& device = devices_[index.row()];

        switch (index.column()) {
            case static_cast<int>(Columns::Id):
                return device.id;
            case static_cast<int>(Columns::Address):
                return device.address.toString();
            case static_cast<int>(Columns::Status):
                return device.status == DeviceStatus::Connected ? QStringLiteral("Connected")
                                                                : QStringLiteral("Disconnected");
            default:
                return {};
        }
    }

    void addDevice(const DeviceInfo& device) {
        beginInsertRows(QModelIndex{}, devices_.size(), devices_.size());
        devices_.push_back(device);
        endInsertRows();
    }

    void setDeviceDisconnected(std::size_t id) {
        auto iter = std::find_if(devices_.begin(), devices_.end(), [id](const auto& device) {
            return device.id == id;
        });
        if (iter != devices_.cend()) {
            const auto row = static_cast<int>(std::distance(devices_.begin(), iter));
            iter->status = DeviceStatus::Disconnected;
            emit dataChanged(index(row, static_cast<int>(Columns::Status)),
                             index(row, static_cast<int>(Columns::Status)),
                             {Qt::DisplayRole});
        }
    }

    void removeDevice(std::size_t id) {
        auto iter = std::find_if(devices_.cbegin(), devices_.cend(), [id](const auto& device) {
            return device.id == id;
        });
        if (iter != devices_.cend()) {
            const auto row = static_cast<int>(std::distance(devices_.cbegin(), iter));
            beginRemoveRows(QModelIndex{}, row, row);
            devices_.erase(iter);
            endRemoveRows();
        }
    }

private:
    enum class Columns {
        Id,
        Address,
        Status,
        ColumnsCount
    };

    std::vector<DeviceInfo> devices_;
};