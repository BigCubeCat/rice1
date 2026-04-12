#pragma once

#include <queue>

#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "hashworker.hpp"


namespace utils {
crack_hash_worker::back::THashWorker body2worker(const QString &body);
};

class TProcessor final : public QObject {
    Q_OBJECT
public:
    explicit TProcessor(QObject *parent = nullptr);
    ~TProcessor() override = default;

    void addTask(const crack_hash_worker::back::THashWorker &worker);

private:
    QTimer m_timer;

    std::queue<crack_hash_worker::back::THashWorker> m_queue;
};
