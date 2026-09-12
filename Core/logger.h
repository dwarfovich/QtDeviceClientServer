#pragma once

#include "log_entry.h"

#include <QObject>
#include <QString>

#include <deque>
#include <mutex>

class Logger : public QObject {
    Q_OBJECT

public:


    void logMessage(const QString& message, std::size_t source = LogEntry::systemId, LogMessageSeverity severity = LogMessageSeverity::Info)
    {
        bool removedOldest = false;
        {
            std::lock_guard lock{mutex_};

            log_.emplace_back(message, QDateTime::currentDateTimeUtc(), source, severity);

            if (log_.size() > maxLogLength_) {
                log_.pop_front();
                removedOldest = true;
            }
        }

        emit messageAdded(removedOldest);
    }

    std::size_t size() const
    {
        std::lock_guard lock{mutex_};
        return log_.size();
    }

    LogEntry logEntry(std::size_t index) const
    {
        std::lock_guard lock{mutex_};
        return log_.at(index);
    }

signals:
    void messageAdded(bool removedOldest);

private:
    mutable std::mutex mutex_;
    std::size_t maxLogLength_ = 10'000;
    std::deque<LogEntry> log_;
};