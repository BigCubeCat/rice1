#pragma once

#include <qjsonobject.h>

namespace dto {
/*!
 * \brief Интерфейс объекта для передачи
 */
class TDtoIface {
public:
    TDtoIface()                             = default;
    TDtoIface(const TDtoIface &)            = default;
    TDtoIface(TDtoIface &&)                 = delete;
    TDtoIface &operator=(const TDtoIface &) = default;
    TDtoIface &operator=(TDtoIface &&)      = delete;
    virtual ~TDtoIface()                    = default;

    [[nodiscard]] virtual QJsonObject serialize() const = 0;
    virtual void deserialize(const QJsonObject &object) = 0;
};
};    // namespace dto
