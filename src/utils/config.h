#pragma once

#include <QString>

namespace Config {
QString getEnv(const QString &key, const QString &default_value);

extern const QString MQTT_BROKER;
extern const int MQTT_PORT;
extern const QString MQTT_TOPIC_VITALS;
extern const int MQTT_QOS;
extern const int STATUS_UPDATE_INTERVAL_MS;
extern const int DATA_STALE_TIMEOUT_MS;
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
} // namespace Config
