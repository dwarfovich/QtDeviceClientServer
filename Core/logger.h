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
        bool removedOldest = false;
        {
            std::lock_guard lock{mutex_};

            log_.push_back(message);

            if (log_.size() > maxLogLength_) {
                log_.pop_front();
                removedOldest = true;
            }
        }

        emit messageAdded(message, removedOldest);
    }

    std::size_t size() const
    {
        std::lock_guard lock{mutex_};
        return log_.size();
    }

    QString message(std::size_t index) const
    {
        std::lock_guard lock{mutex_};
        return log_.at(index);
    }

signals:
    void messageAdded(const QString& message, bool removedOldest);

private:
    mutable std::mutex mutex_;
    std::size_t maxLogLength_ = 10'000;
    std::deque<QString> log_;
};