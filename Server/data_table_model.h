#pragma once

#include "device_data.h"
#include "device_info.h"

#include <QAbstractTableModel>

#include <unordered_map>
#include <variant>
#include <vector>

class DataTableModel : public QAbstractTableModel {
    Q_OBJECT

    enum class Column {
        Id,
        Bandwidth,
        Latency,
        PacketLoss,
        Uptime,
        CpuUsage,
        MemoryUsage,
        SignalLampState,
        IsActive,
        Log,
        ColumnsCount
    };

public:
    DataTableModel(QObject* parent = nullptr) : QAbstractTableModel{parent} {}

    int rowCount(const QModelIndex& parent = {}) const override
    {
        return ids_.size();
    }

    int columnCount(const QModelIndex& parent = {}) const override
    {
        return toInt(Column::ColumnsCount);
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(ids_.size())) {
            return {};
        }
        if (role != Qt::DisplayRole) {
            return {};
        }

        const auto id = ids_[index.row()];
        if (columnType(index.column()) == Column::Id) {
            return QVariant::fromValue(id);
        }

        const auto iter = idToData_.find(id);
        if (iter == idToData_.end()) {
            return {};
        }

        const auto propertyName = columnPropertyName(columnType(index.column()));

        return iter->second.properties_.value(propertyName);
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
            return {};
        }

        switch (columnType(section)) {
            case Column::Id:
                return "ID";
            case Column::Bandwidth:
                return "Bandwidth";
            case Column::Latency:
                return "Latency";
            case Column::PacketLoss:
                return "Packet loss";
            case Column::Uptime:
                return "Uptime";
            case Column::CpuUsage:
                return "CpuUsage";
            case Column::MemoryUsage:
                return "MemoryUsage";
            case Column::SignalLampState:
                return "SignalLampState";
            case Column::IsActive:
                return "IsActive";
            case Column::Log:
                return "Log";
            default:
                return {};
        }
    }

    void addDevice(std::size_t id)
    {
        if (idToData_.contains(id)) {
            return;
        }

        const std::size_t row = ids_.size();

        beginInsertRows(QModelIndex{}, row, row);
        idToData_.emplace(id, DeviceData{});
        ids_.push_back(id);
        idToRow_.emplace(id, row);
        endInsertRows();
    }

    void removeDevice(std::size_t id)
    {
        const std::size_t row = idToRow_.at(id);

        beginRemoveRows(QModelIndex{}, row, row);
        idToData_.erase(id);
        idToRow_.erase(id);
        for (auto iter = ids_.begin() + row + 1; iter != ids_.end(); ++iter) {
            --idToRow_.at(*iter);
        }
        ids_.erase(ids_.begin() + row);
        endRemoveRows();
    }

    void updateDevice(std::size_t id, const device_message::Message& message)
    {
        auto iter = idToData_.find(id);
        if (iter == idToData_.end()) {
            return;
        }

        auto& data = iter->second;
        const std::size_t row = idToRow_.at(id);
        bool changed = false;

        std::visit(
            [&data, &changed](const auto& messageData) {
                using MessageType = std::decay_t<decltype(messageData)>;

                if constexpr (requires { MessageType::staticMetaObject; }) {
                    const QMetaObject& meta = MessageType::staticMetaObject;
                    for (int i = meta.propertyOffset(); i < meta.propertyCount(); ++i) {
                        const QMetaProperty property = meta.property(i);
                        const QVariant value = property.readOnGadget(&messageData);
                        const QString name = property.name();
                        if (data.properties_.value(name) != value) {
                            data.properties_[name] = value;
                            changed = true;
                        }
                    }
                }
            },
            message.data());

        if (changed) {
            emit dataChanged(index(row, 0), index(row, columnCount() - 1));
        }
    }

    static constexpr int idColumn()
    {
        return static_cast<int>(Column::Id);
    }

    static constexpr int signalLampColumn()
    {
        return static_cast<int>(Column::SignalLampState);
    }

    bool signalLampState(std::size_t id) const{
        const auto iter = idToData_.find(id);
        if (iter == idToData_.end()) {
            return false;
        }

        return iter->second.properties_.value("signalLampState").toBool();
    }

    const std::vector<size_t>& ids() const{
        return ids_;
    }

 private:
    std::vector<size_t> ids_;
    std::unordered_map<size_t, DeviceData> idToData_;
    std::unordered_map<size_t, std::size_t> idToRow_;

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

    QString columnPropertyName(Column column) const
    {
        switch (column) {
            case Column::Id:
                return {};
            case Column::Bandwidth:
                return QStringLiteral("bandwidth");
            case Column::Latency:
                return QStringLiteral("latency");
            case Column::PacketLoss:
                return QStringLiteral("packetLoss");
            case Column::Uptime:
                return QStringLiteral("uptime");
            case Column::CpuUsage:
                return QStringLiteral("cpuUsage");
            case Column::MemoryUsage:
                return QStringLiteral("memoryUsage");
            case Column::SignalLampState:
                return QStringLiteral("signalLampState");
            case Column::IsActive:
                return QStringLiteral("isActive");
            case Column::Log:
                return QStringLiteral("message");
            default:
                return {};
        }
    }
};