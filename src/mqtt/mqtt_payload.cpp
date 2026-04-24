#include "mqtt_payload.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <stdexcept>

namespace MqttPayload {

/**
 * @brief Parse a JSON vital-signs payload into a VitalReading.
 *
 * @param json_str UTF-8 JSON string from the MQTT broker.
 * @return Populated VitalReading.
 * @throws std::runtime_error  if the JSON is syntactically invalid.
 * @throws std::out_of_range   if a required field is absent.
 */
VitalReading parseVital(const QString &json_str) {
  const QJsonDocument doc = QJsonDocument::fromJson(json_str.toUtf8());
  if (doc.isNull() || !doc.isObject()) {
    throw std::runtime_error("Invalid JSON payload");
  }

  const QJsonObject obj = doc.object();

  // Verify every required field is present
  static const char *required[] = {"timestamp", "hr",     "bp_sys",
                                   "bp_dia",    "o2_sat", "temperature",
                                   "quality",   "source", nullptr};
  for (int i = 0; required[i] != nullptr; ++i) {
    if (!obj.contains(required[i])) {
      throw std::out_of_range(std::string("Missing field: ") + required[i]);
    }
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
