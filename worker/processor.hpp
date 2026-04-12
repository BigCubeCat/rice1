#pragma once

#include <queue>

#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "worker.hpp"

#include "back/hashworker.hpp"



namespace utils {
crack_hash_worker::back::THashWorker body2worker(const QString &body);
};

class TProcessor final : public QObject {
    Q_OBJECT
public:
    explicit TProcessor(QString url, QObject *parent = nullptr);

    void addTask(const crack_hash_worker::back::THashWorker &worker);


private slots:
    void process();

public:
private:
    QTimer m_timer;
    std::queue<crack_hash_worker::back::THashWorker> m_queue;
    crack_hash_worker::back::THashWorker *m_current = nullptr;
    QString m_managerUrl;
    TNetworkWorker *m_worker;
};
