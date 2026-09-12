#pragma once

#include "Core/logger.h"

#include <QAbstractListModel>
#include <QColor>
#include <QApplication>
#include <QPalette>

#include <memory>

class LogModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit LogModel(const std::shared_ptr<Logger>& logger, QObject* parent = nullptr) : QAbstractListModel{parent}, logger_{logger}
    {
        Q_ASSERT(logger);

       connect(logger_.get(), &Logger::messageAdded, this, [this](bool oldestRemoved) {
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

     QColor tint(const QColor& base, const QColor& tintColor, double amount) const 
    {
        return QColor::fromRgbF(base.redF() * (1.0 - amount) + tintColor.redF() * amount,
                                base.greenF() * (1.0 - amount) + tintColor.greenF() * amount,
                                base.blueF() * (1.0 - amount) + tintColor.blueF() * amount);
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid() || index.row() >= rowCount({})) {
            return {};
        }

        const auto entry = logger_->logEntry(index.row());

        if (role == Qt::DisplayRole) {
            return QString{"[%1] | %2 | [%3] | %4"}
                .arg(toString(entry.severity).leftJustified(8, ' '))
                .arg(entry.time.toString("yyyy-MM-dd hh:mm:ss"))
                .arg(entry.source)
                .arg(entry.message);
        }

        if (role == Qt::BackgroundRole) {
            const QColor baseColor = QApplication::palette().color(QPalette::Base);

            QColor result = QColor::fromRgbF(baseColor.redF() * 0.9 + 1.0 * 0.1,
                                             baseColor.greenF() * 0.9,
                                             baseColor.blueF() * 0.9);
            switch (entry.severity) {
                case LogMessageSeverity::Critical:
                case LogMessageSeverity::High:
                    return tint(baseColor, Qt::red, 0.1);
                case LogMessageSeverity::Medium:
                    return tint(baseColor, Qt::yellow, 0.1);

                default:
                    return {};
            }
        }

        return {};
    }

   


private:
    std::shared_ptr<Logger> logger_;
};