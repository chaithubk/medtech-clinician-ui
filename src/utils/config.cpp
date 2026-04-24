#include "config.h"

#include <QByteArray>
#include <QProcessEnvironment>

namespace {
int envToInt(const QString &key, int default_value) {
  bool ok = false;
  const int value =
      QProcessEnvironment::systemEnvironment().value(key).toInt(&ok);
  return ok ? value : default_value;
}
} // namespace

namespace Config {
QString getEnv(const QString &key, const QString &default_value) {
  return QProcessEnvironment::systemEnvironment().value(key, default_value);
}

const QString MQTT_BROKER = getEnv("MQTT_BROKER", "localhost");
const int MQTT_PORT = envToInt("MQTT_PORT", 1883);
const QString MQTT_TOPIC_VITALS =
    getEnv("MQTT_TOPIC_VITALS", "medtech/vitals/latest");
const int MQTT_QOS = envToInt("MQTT_QOS", 1);
const int STATUS_UPDATE_INTERVAL_MS =
    envToInt("STATUS_UPDATE_INTERVAL_MS", 1000);
const int DATA_STALE_TIMEOUT_MS = envToInt("DATA_STALE_TIMEOUT_MS", 5000);
const int WINDOW_WIDTH = envToInt("WINDOW_WIDTH", 1920);
const int WINDOW_HEIGHT = envToInt("WINDOW_HEIGHT", 1080);
} // namespace Config
