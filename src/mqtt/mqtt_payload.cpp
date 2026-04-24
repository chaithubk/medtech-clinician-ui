#include "mqtt_payload.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <stdexcept>

namespace MqttPayload {
VitalReading parseVital(const QString &json_str) {
  const QJsonDocument doc = QJsonDocument::fromJson(json_str.toUtf8());
  if (!doc.isObject()) {
    throw std::runtime_error("Invalid JSON payload");
  }

  const QJsonObject obj = doc.object();
  if (!obj.contains("timestamp")) {
    throw std::out_of_range("Missing timestamp");
  }

  VitalReading reading;
  reading.timestamp = static_cast<qint64>(obj.value("timestamp").toDouble(0));
  reading.hr = static_cast<float>(obj.value("hr").toDouble(0));
  reading.bp_sys = static_cast<float>(obj.value("bp_sys").toDouble(0));
  reading.bp_dia = static_cast<float>(obj.value("bp_dia").toDouble(0));
  reading.o2_sat = static_cast<float>(obj.value("o2_sat").toDouble(0));
  reading.temperature =
      static_cast<float>(obj.value("temperature").toDouble(0));
  reading.quality = obj.value("quality").toInt(0);
  reading.source = obj.value("source").toString();
  return reading;
}
} // namespace MqttPayload
