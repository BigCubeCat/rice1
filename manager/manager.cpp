#include "manager.hpp"
//
#include <QJsonObject>
#include <algorithm>
#include <iostream>

#include <qdebug.h>
#include <qhttpserverrequest.h>
#include <qhttpserverresponse.h>
#include <qlogging.h>

#include "request.hxx"
#include "utils.hpp"
#include "worker.hpp"

#include "dto/crack.hpp"
#include "dto/status.hpp"

TManager::TManager(QObject *parent)
    : QObject(parent), m_worker(new TNetworkWorker(this)) {
    QObject::connect(&m_timer, &QTimer::timeout, this, &TManager::onTimer);
    m_timer.setInterval(100);
    m_timer.start();
}

QString TManager::addTask(const Task &task) {
    Task t = task;
    while (true) {
        QString requestId = utils::generateUUID();
        if (!m_taskMap.contains(requestId)) {
            t.requestId          = requestId;
            m_taskMap[requestId] = t;
            m_tasksQueue.push(requestId);
            return requestId;
        }
    }
}

EStatus TManager::taskStatus(const QString &requestId) const {
    if (m_taskMap.contains(requestId)) {
        return m_taskMap.at(requestId).status;
    }
    return EStatus::STATUS_FAILED;
}

QString TManager::currentTaskId() const {
    return m_currentTaskId;
}

QHttpServerResponse TManager::statusHandler(const QHttpServerRequest &request) {
    QUrl url = request.url();
    QUrlQuery query(url);
    QString requestId = query.queryItemValue("requestId");
    if (!m_taskMap.contains(requestId)) {
        return { QHttpServerResponder::StatusCode::NotFound };
    }
    const auto &task = m_taskMap.find(requestId)->second;
    dto::TStatusResponse status(statusToString(task.status));
    if (task.status == EStatus::STATUS_PENDING
        || task.status == EStatus::STATUS_FAILED) {
        status.setData(std::nullopt);
    }
    else {
        status.setData(task.answers);
    }
    QJsonObject json = status.serialize();
    QJsonDocument doc(json);
    return { doc.toJson() };
}

QHttpServerResponse TManager::crackHandler(const QHttpServerRequest &request) {
    Task task;
    const auto body    = request.body();
    const auto jsonDoc = QJsonDocument::fromJson(body);
    if (!jsonDoc.isObject()) {
        return { QHttpServerResponder::StatusCode::BadRequest };
    }
    const auto jsonObj = jsonDoc.object();
    dto::TCrackRequest requestDto;
    requestDto.deserialize(jsonObj);
    task.hash            = requestDto.hash();
    task.maxLength       = requestDto.maxLength();
    task.status          = EStatus::STATUS_PENDING;
    const auto newTaskId = addTask(task);
    dto::TCrackResponse response;
    response.setRequestId(newTaskId);

    QJsonDocument doc(response.serialize());
    return { doc.toJson() };
}

QHttpServerResponse
TManager::internalHandler(const QHttpServerRequest &request) {
    const auto body               = request.body();
    const auto tp                 = utils::body2taskPart(body);
    m_currentParts[tp.partNumber] = tp;
    m_done[tp.partNumber]         = true;
    if (std::all_of(m_done.begin(), m_done.end(), [](bool b) { return b; })) {
        // ответ готов
        QStringList answers;
        for (const auto &part : m_currentParts)
            answers += part.answers;
        m_taskMap[m_currentTaskId].answers = answers;
        m_taskMap[m_currentTaskId].status  = STATUS_COMPLETED;
    }
    return "";
}

void TManager::onTimer() {
    const auto value = m_taskMap.find(m_currentTaskId);
    if (m_currentTaskId == "" || value == m_taskMap.end()) {
        nextTask();
        return;
    }
    const auto status = value->second.status;
    if (status == EStatus::STATUS_COMPLETED
        || status == EStatus::STATUS_FAILED) {
        nextTask();
        return;
    }
}

void TManager::nextTask() {
    if (m_tasksQueue.empty()) {
        m_currentTaskId = "";
        return;
    }
    m_currentTaskId = m_tasksQueue.front();
    m_tasksQueue.pop();
    std::fill(m_currentParts.begin(), m_currentParts.end(), TaskPart {});
    std::fill(m_done.begin(), m_done.end(), false);
    const auto value = m_taskMap.find(m_currentTaskId);
    if (value == m_taskMap.end()) {
        return;
    }
    const auto &task = value->second;
    const auto xmlBodys =
        utils::task2bodys(task, static_cast<int>(m_workers.size()));
    for (int i = 0; i < xmlBodys.size(); i++) {
        m_worker->sendRequest(
            m_workers[i] + "/internal/api/worker/hash/crack/task", xmlBodys[i]
        );
    }
}
