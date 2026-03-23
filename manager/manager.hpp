#pragma once

#include <QHttpServer>
#include <QObject>
#include <queue>

#include <qtmetamacros.h>
#include <qxmlstream.h>


enum EStatus : uint8_t {
    STATUS_PENDING     = 0,
    STATUS_IN_PROGRESS = 1,
    STATUS_COMPLETED   = 2,
    STATUS_FAILED      = 3
};

struct Task {
    QString hash;
    QString requestId;
    int maxLength;
    EStatus status;
};

class TManager final : public QObject {
    Q_OBJECT
public:
    QString addTask(const Task &task);
    EStatus taskStatus(const QString &requestId);
    QString currentTaskId() const;

private:
    std::unordered_map<QString, Task> m_taskMap;
    std::queue<QString> m_taskQueue;
};
