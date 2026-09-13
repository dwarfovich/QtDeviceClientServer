#pragma once

#include <QColor>

inline QColor tint(const QColor& base, const QColor& tintColor, double amount)
{
    return QColor::fromRgbF(base.redF() * (1.0 - amount) + tintColor.redF() * amount,
                            base.greenF() * (1.0 - amount) + tintColor.greenF() * amount,
                            base.blueF() * (1.0 - amount) + tintColor.blueF() * amount);
}