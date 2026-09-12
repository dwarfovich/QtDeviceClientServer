#pragma once

#include "Core/logger.h"

#include <QAbstractListModel>

#include <memory>

class LogModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit LogModel(const std::shared_ptr<Logger>& logger, QObject* parent = nullptr) : QAbstractListModel{parent}, logger_{logger}
    {
        Q_ASSERT(logger);

       connect(logger_.get(), &Logger::messageAdded, this, [this](const QString&, bool oldestRemoved) {
            if (oldestRemoved) {
                beginRemoveRows({}, 0, 0);
                endRemoveRows();
            }

            const int row = rowCount({});

            beginInsertRows({}, row, row);
            endInsertRows();
        });
    }

    int rowCount(const QModelIndex& parent) const override
    {
        if (parent.isValid()) {
            return 0;
        }

        return static_cast<int>(logger_->size());
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid() || index.row() >= rowCount({})) {
            return {};
        }

        if (role == Qt::DisplayRole) {
            return logger_->message(index.row());
        }

        return {};
    }

private:
    std::shared_ptr<Logger> logger_;
};