#pragma once

#include <QString>

namespace Logger {
void info(const QString &message);
void warn(const QString &message);
void error(const QString &message);
void debug(const QString &message);
} // namespace Logger
