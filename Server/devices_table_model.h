#pragma once

#include "device_info.h"

#include <QAbstractTableModel>

#include <vector>

class DevicesTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    DevicesTableModel(QObject* parent = nullptr) : QAbstractTableModel{parent} {}

    const auto& devices() const {
        return devices_;
    }
    int rowCount(const QModelIndex& parent = {}) const override
    {
        return devices_.size();
    }

    int columnCount(const QModelIndex& parent = {}) const override
    {
        return toInt(Column::ColumnsCount);
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(devices_.size())) {
            return {};
        }

        if (role != Qt::DisplayRole) {
            return {};
        }

        const auto& device = devices_[index.row()];

        switch (columnType(index.column())) {
            case Column::Id:
                return device.id;
            case Column::Address:
                return device.address.toString();
            case Column::Status:
                return device.status == DeviceStatus::Connected ? QStringLiteral("Connected")
                                                                : QStringLiteral("Disconnected");
            default:
                return {};
        }
    }

    static constexpr int idColumn()
    {
        return static_cast<int>(Column::Id);
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
            return {};
        }

        switch (columnType(section)) {
            case Column::Id:
                return "ID";
            case Column::Address:
                return "Address";
            case Column::Status:
                return "Status";
            default:
                return {};
        }
    }

    void addDevice(const DeviceInfo& device)
    {
        beginInsertRows(QModelIndex{}, devices_.size(), devices_.size());
        devices_.push_back(device);
        endInsertRows();
    }

    void setDeviceDisconnected(std::size_t id)
    {
        auto iter = std::find_if(devices_.begin(), devices_.end(), [id](const auto& device) {
            return device.id == id;
        });
        if (iter != devices_.cend()) {
            const auto row = static_cast<int>(std::distance(devices_.begin(), iter));
            iter->status = DeviceStatus::Disconnected;
            emit dataChanged(index(row, toInt(Column::Status)),
                             index(row, toInt(Column::Status)),
                             {Qt::DisplayRole});
        }
    }

    void removeDevice(std::size_t id)
    {
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
    enum class Column {
        Id,
        Address,
        Status,
        
        ColumnsCount
    };

    std::vector<DeviceInfo> devices_;

private:  // methods
    int toInt(Column column) const
    {
        return static_cast<int>(column);
    }

    Column columnType(int index) const
    {
        Q_ASSERT(index < toInt(Column::ColumnsCount));

        return static_cast<Column>(index);
    }
};