#pragma once

#include <QTimer>
#include <queue>

#include <qobject.h>
#include <qtmetamacros.h>

#include "hashworker.hpp"

namespace utils {
crack_hash_worker::back::THashWorker body2worker(const QString &body);
};

class TProcessor final : public QObject {
    Q_OBJECT
public:
    explicit TProcessor(QObject *parent = nullptr);
    ~TProcessor();

    void addTask(const crack_hash_worker::back::THashWorker &worker);
private slots:
    void process();

private:
    TProcessor(const TProcessor &)            = default;
    TProcessor(TProcessor &&)                 = delete;
    TProcessor &operator=(const TProcessor &) = default;
    TProcessor &operator=(TProcessor &&)      = delete;
    QTimer m_timer;

    std::queue<crack_hash_worker::back::THashWorker> m_queue;
};
