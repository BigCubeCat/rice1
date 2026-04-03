#pragma once

#include <QHttpServer>
#include <QObject>
#include <queue>

#include <qhttpserverrequest.h>
#include <qtmetamacros.h>
#include <qxmlstream.h>

#include "task.hpp"

class TManager final : public QObject {
    Q_OBJECT
public:
    explicit TManager(QObject *parent = nullptr);
    TManager(const TManager &)            = delete;
    TManager(TManager &&)                 = delete;
    TManager &operator=(const TManager &) = delete;
    TManager &operator=(TManager &&)      = delete;

    QString addTask(const Task &task);
    EStatus taskStatus(const QString &requestId) const;
    QString currentTaskId() const;

    QHttpServerResponse statusHandler(const QHttpServerRequest &request);
    QHttpServerResponse crackHandler(const QHttpServerRequest &request);

    ~TManager() override = default;

private:
    std::unordered_map<QString, Task> m_taskMap;
    std::queue<QString> m_taskQueue;
};
