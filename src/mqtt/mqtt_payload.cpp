#include "mqtt_payload.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <stdexcept>

namespace MqttPayload {

/**
 * @brief Parse a JSON vital-signs payload into a VitalReading.
 *
 * Enforces the MedTech Vitals Telemetry Contract v2.0 (see
 * contracts/vitals/v2.0.json).  Rejects payloads where "version" is absent
 * or not equal to "2.0" by throwing std::runtime_error — callers should log
 * a warning and discard the message.
 *
 * @param json_str UTF-8 JSON string from the MQTT broker.
 * @return Populated VitalReading.
 * @throws std::runtime_error  if the JSON is syntactically invalid or the
 *                              schema version does not match "2.0".
 * @throws std::out_of_range   if a required field is absent.
 */
VitalReading parseVital(const QString &json_str) {
  const QJsonDocument doc = QJsonDocument::fromJson(json_str.toUtf8());
  if (doc.isNull() || !doc.isObject()) {
    throw std::runtime_error("Invalid JSON payload");
  }

  const QJsonObject obj = doc.object();

  // Strict version enforcement — drop payloads that do not match v2.0
  if (!obj.contains("version") || obj.value("version").toString() != "2.0") {
    throw std::runtime_error(
        "Unsupported or missing payload version (expected \"2.0\"); "
        "message dropped");
  }

  // Verify every required field (per v2.0 schema) is present.
  // sepsis_onset_ts is required but may be JSON null — obj.contains() is
  // still true for a null-valued key.
  static const char *required[] = {
      "version",         "patient_id",  "scenario",         "scenario_stage",
      "timestamp",       "hr",          "bp_sys",           "bp_dia",
      "o2_sat",          "temperature", "respiratory_rate", "wbc",
      "lactate",         "sirs_score",  "qsofa_score",      "sepsis_stage",
      "sepsis_onset_ts", "quality",     "source",           nullptr};
  for (int i = 0; required[i] != nullptr; ++i) {
    if (!obj.contains(required[i])) {
      throw std::out_of_range(std::string("Missing field: ") + required[i]);
    }
  }

  VitalReading reading;
  reading.version = obj.value("version").toString();
  reading.patient_id = obj.value("patient_id").toString();
  reading.scenario = obj.value("scenario").toString();
  reading.scenario_stage = obj.value("scenario_stage").toString();
  reading.timestamp = static_cast<qint64>(obj.value("timestamp").toDouble(0));
  reading.hr = static_cast<float>(obj.value("hr").toDouble(0));
  reading.bp_sys = static_cast<float>(obj.value("bp_sys").toDouble(0));
  reading.bp_dia = static_cast<float>(obj.value("bp_dia").toDouble(0));
  reading.o2_sat = static_cast<float>(obj.value("o2_sat").toDouble(0));
  reading.temperature =
      static_cast<float>(obj.value("temperature").toDouble(0));
  reading.respiratory_rate =
      static_cast<float>(obj.value("respiratory_rate").toDouble(0));
  reading.wbc = static_cast<float>(obj.value("wbc").toDouble(0));
  reading.lactate = static_cast<float>(obj.value("lactate").toDouble(0));
  reading.sirs_score = obj.value("sirs_score").toInt(0);
  reading.qsofa_score = obj.value("qsofa_score").toInt(0);
  reading.sepsis_stage = obj.value("sepsis_stage").toString();

  // sepsis_onset_ts is nullable — represent absent onset as std::nullopt
  const QJsonValue onset = obj.value("sepsis_onset_ts");
  if (onset.isNull()) {
    reading.sepsis_onset_ts = std::nullopt;
  } else {
    reading.sepsis_onset_ts = static_cast<qint64>(onset.toDouble(0));
  }

  reading.quality = obj.value("quality").toString();
  reading.source = obj.value("source").toString();
  return reading;
}

} // namespace MqttPayload
