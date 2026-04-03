#pragma once

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
