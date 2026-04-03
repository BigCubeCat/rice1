#pragma once

#include <qjsonarray.h>
#include <qjsonobject.h>

#include "dto.hpp"

namespace dto {
class TCrackRequest final : public TDtoIface {
public:
    TCrackRequest()                                 = default;
    TCrackRequest(const TCrackRequest &)            = default;
    TCrackRequest(TCrackRequest &&)                 = delete;
    TCrackRequest &operator=(const TCrackRequest &) = default;
    TCrackRequest &operator=(TCrackRequest &&)      = delete;
    ~TCrackRequest() override                       = default;

    [[nodiscard]] QJsonObject serialize() const override {
        throw std::runtime_error("Not implemented");
        return {};
    }
    void deserialize(const QJsonObject &object) override {
        m_hash      = object["hash"].toString();
        m_maxLength = object["maxLength"].toInt();
    }

    [[nodiscard]] QString hash() const {
        return m_hash;
    }
    [[nodiscard]] int maxLength() const {
        return m_maxLength;
    }

private:
    QString m_hash  = "";
    int m_maxLength = 0;
};

class TCrackResponse final : public TDtoIface {
public:
    TCrackResponse()                                  = default;
    TCrackResponse(const TCrackResponse &)            = default;
    TCrackResponse(TCrackResponse &&)                 = delete;
    TCrackResponse &operator=(const TCrackResponse &) = default;
    TCrackResponse &operator=(TCrackResponse &&)      = delete;
    ~TCrackResponse() override                        = default;

    [[nodiscard]] QJsonObject serialize() const override {
        QJsonObject object;
        object["requestId"] = m_requestId;
        return object;
    }
    void deserialize(const QJsonObject &object) override {
        throw std::runtime_error("Not implemented");
    }

    void setRequestId(const QString &requestId) {
        m_requestId = requestId;
    }

private:
    QString m_requestId;
};
};    // namespace dto
