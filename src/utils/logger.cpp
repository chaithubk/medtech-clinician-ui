#include "logger.h"

#include <QDateTime>
#include <QDebug>

namespace {
QString stamp(const QString &level, const QString &message) {
  const QString ts = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
  return QString("[%1] [%2] %3").arg(ts, level, message);
}
} // namespace

namespace Logger {
void info(const QString &message) {
  qInfo().noquote() << stamp("INFO", message);
}
void warn(const QString &message) {
  qWarning().noquote() << stamp("WARN", message);
}
void error(const QString &message) {
  qCritical().noquote() << stamp("ERROR", message);
}
void debug(const QString &message) {
  qDebug().noquote() << stamp("DEBUG", message);
}
} // namespace Logger
