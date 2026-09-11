#pragma once

#include <QObject>
#include <QString>

#include <deque>
#include <mutex>

class Logger : public QObject {
    Q_OBJECT

public:
    void logMessage(const QString& message)
    {
        {
            std::lock_guard lock{mutex_};
            log_.push_back(message);
            if (log_.size() > maxLogLength_) {
                log_.pop_front();
            }
        }
        emit messageAdded(message);
    }

signals:
    void messageAdded(const QString& message);

private:
    mutable std::mutex mutex_;
    std::size_t maxLogLength_ = 10000;
    std::deque<QString> log_;
};