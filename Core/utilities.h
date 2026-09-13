#pragma once

#include "text_generator.h"

#include <QRandomGenerator>

template <typename T>
concept Enum = std::is_enum_v<T>;

template <Enum T>
std::underlying_type_t<T> toUnderlyingType(T value)
{
    return static_cast<std::underlying_type_t<T>>(value);
}

inline double randomDouble2Precision(double min, double max)
{
    Q_ASSERT(min < max);

    return std::round((min + QRandomGenerator::global()->generateDouble() * (max - min)) * 100.) / 100.;
}

template <std::integral T>
T randomInt(T min, T max)
{
    Q_ASSERT(min < max);

    return QRandomGenerator::global()->bounded(min, max);
}

inline QString randomText()
{
    return generateText(randomInt(0, 3));
}

inline LogMessageSeverity randomLogMessageSeverity()
{
    const auto randomNumber = randomInt(toUnderlyingType(LogMessageSeverity::Info),
                                        toUnderlyingType(LogMessageSeverity::Unknown));

    return LogMessageSeverity(randomNumber);
}
