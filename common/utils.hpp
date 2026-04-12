#pragma once

#include <QString>

#include "task.hpp"

namespace utils {
QString generateUUID();
QString task2body(const Task &task, int size, int rank);
QStringList task2bodys(const Task &task, int size);
TaskPart body2taskPart(const QString &body);
std::optional<QString> getEnv(const std::string &name);
}    // namespace utils
