#pragma once

#include <QHash>
#include <QString>
#include <QVector>

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
    QVector<QString> answers;
};

struct TaskPart {
    int partNumber = 0;
    QVector<QString> answers;
};

inline QString statusToString(EStatus status) {
    switch (status) {
    case STATUS_PENDING:
        return "PENDING";
    case STATUS_IN_PROGRESS:
        return "IN_PROGRESS";
    case STATUS_COMPLETED:
        return "COMPLETED";
    case STATUS_FAILED:
        return "FAILED";
    default:
        return "UNKNOWN";
    }
}

inline EStatus stringToStatus(const QString &string) {
    static QHash<QString, EStatus> ht = {
        {     "PENDING",     STATUS_PENDING },
        { "IN_PROGRESS", STATUS_IN_PROGRESS },
        {   "COMPLETED",   STATUS_COMPLETED },
        {      "FAILED",      STATUS_FAILED },
        {     "UNKNOWN",      STATUS_FAILED }
    };
    const auto result = ht.constFind(string);
    if (result == ht.end())
        return STATUS_FAILED;
    return result.value();
}
