#pragma once

#include <QString>

#include <cinttypes>

enum class LogMessageSeverity : int {
    Info,
    Low,
    Medium,
    High,
    Critical,
    Unknown
};

inline QString toString(LogMessageSeverity severity){
    switch (severity) {
        case LogMessageSeverity::Info:
            return QStringLiteral("Info");
        case LogMessageSeverity::Low:
            return QStringLiteral("Low");
        case LogMessageSeverity::Medium:
            return QStringLiteral("Medium");
        case LogMessageSeverity::High:
            return QStringLiteral("HIGH");
        case LogMessageSeverity::Critical:
            return QStringLiteral("CRITICAL");
        default:
            return QStringLiteral("Unknown");
    }
}
