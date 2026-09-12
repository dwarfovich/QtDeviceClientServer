#pragma once

#include "log_message_severity.h"

#include <QString>
#include <QDateTime>

struct LogEntry{
    inline static constexpr std::size_t systemId = 0;

    QString message;
    QDateTime time;
    std::size_t source = systemId;
    LogMessageSeverity severity = LogMessageSeverity::Info;
};