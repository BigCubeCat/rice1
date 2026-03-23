#include "manager.hpp"
//
#include <random>

namespace {
QString generateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

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

QString TManager::addTask(const Task &task) {
    while (true) {
        QString requestId = generateUUID();
        if (!m_taskMap.contains(requestId)) {
            m_taskMap[requestId] = task;
            m_taskQueue.push(requestId);
            return requestId;
        }
    }
}

EStatus TManager::taskStatus(const QString &requestId) {
    if (m_taskMap.contains(requestId)) {
        return m_taskMap[requestId].status;
    }
    return EStatus::STATUS_FAILED;
}

QString TManager::currentTaskId() const {
    if (!m_taskQueue.empty()) {
        return m_taskQueue.front();
    }
    return {};
}
