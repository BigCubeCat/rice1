#pragma once

#include <QHttpServer>
#include <QObject>
#include <QTimer>
#include <queue>

#include <qhttpserverrequest.h>
#include <qtmetamacros.h>
#include <qxmlstream.h>

#include "task.hpp"
#include "worker.hpp"

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
    void setUrls(const QStringList &urls) {
        m_workers = urls;
    }

    QHttpServerResponse statusHandler(const QHttpServerRequest &request);
    QHttpServerResponse crackHandler(const QHttpServerRequest &request);

    ~TManager() override = default;

private slots:
    void onTimer();

public:
private:
    std::unordered_map<QString, Task> m_taskMap;
    std::queue<QString> m_tasksQueue;
    QString m_currentTaskId;
    TWorker *m_worker = nullptr;
    QTimer m_timer;
    /// ссылки на активных воркеров
    QStringList m_workers;

    void nextTask();
};
