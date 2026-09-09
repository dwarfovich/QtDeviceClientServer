#pragma once

#include <QAbstractTableModel>

class DevicesTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DevicesTableModel(QObject* parent = nullptr) : QAbstractTableModel { parent } {}

    int      rowCount(const QModelIndex& parent = {}) const override {return 1;}
    int      columnCount(const QModelIndex& parent = {}) const override {return 1;}
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {return {}; }
};