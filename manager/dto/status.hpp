#pragma once

#include <stdexcept>
#include <utility>

#include <qjsonarray.h>
#include <qjsonobject.h>

#include "dto.hpp"

namespace dto {
class TStatusResponse final : public TDtoIface {
public:
    TStatusResponse(QString status) : m_status(std::move(status)) { }
    TStatusResponse(const TStatusResponse &)            = default;
    TStatusResponse(TStatusResponse &&)                 = delete;
    TStatusResponse &operator=(const TStatusResponse &) = default;
    TStatusResponse &operator=(TStatusResponse &&)      = delete;
    ~TStatusResponse() override                         = default;

    [[nodiscard]] QJsonObject serialize() const override {
        QJsonObject object;
        object["status"] = m_status;
        if (!m_data.has_value())
            object["data"] = QJsonValue::Null;
        else {
            QJsonArray array;
            const auto data = m_data.value();
            for (const auto &value : data)
                array.append(value);
            object["data"] = array;
        }
        return object;
    }
    void deserialize(const QJsonObject &object) override {
        throw std::runtime_error("Not implemented");
    }

    void setData(const std::optional<QStringList> &data) {
        m_data = data;
    }

private:
    QString m_status;
    std::optional<QStringList> m_data;
};
};    // namespace dto
