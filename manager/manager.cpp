#include "manager.hpp"
//
#include <QJsonObject>
#include <random>

#include <qdebug.h>
#include <qhttpserverrequest.h>
#include <qhttpserverresponse.h>

#include "dto/crack.hpp"
#include "dto/status.hpp"

namespace {
QString generateUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::string hexChars = "0123456789abcdef";
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < 8; i++) {
        ss << hexChars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 4; i++) {
        ss << hexChars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 4; i++) {
        ss << hexChars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 4; i++) {
        ss << hexChars[dis(gen)];
    }
    ss << "-";
    for (int i = 0; i < 12; i++) {
        ss << hexChars[dis(gen)];
    }
    return QString::fromStdString(ss.str());
}
};    // namespace

TManager::TManager(QObject *parent) : QObject(parent) { }

QString TManager::addTask(const Task &task) {
    Task t = task;
    while (true) {
        QString requestId = generateUUID();
        if (!m_taskMap.contains(requestId)) {
            t.requestId          = requestId;
            m_taskMap[requestId] = t;
            m_taskQueue.push(requestId);
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
    if (!m_taskQueue.empty()) {
        return m_taskQueue.front();
    }
    return {};
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
    qDebug() << json;
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
