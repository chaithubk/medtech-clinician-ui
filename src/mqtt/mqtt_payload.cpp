#include "mqtt_payload.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <cmath>
#include <stdexcept>

namespace MqttPayload {
namespace {
QJsonObject g_runtime_schema;
QString g_runtime_schema_path;
constexpr double kMaxSafeInteger = 9007199254740991.0; // 2^53 - 1
bool isInteger(const QJsonValue &value);

QString describeValueType(const QJsonValue &value) {
  if (value.isNull()) {
    return "null";
  }
  if (value.isBool()) {
    return "boolean";
  }
  if (value.isString()) {
    return "string";
  }
  if (value.isArray()) {
    return "array";
  }
  if (value.isObject()) {
    return "object";
  }
  if (value.isDouble()) {
    if (isInteger(value)) {
      return "integer";
    }
    return "number";
  }
  return "unknown";
}

bool isInteger(const QJsonValue &value) {
  if (!value.isDouble()) {
    return false;
  }
  const double n = value.toDouble();
  return std::isfinite(n) && std::abs(n) <= kMaxSafeInteger &&
         std::trunc(n) == n;
}

bool validateType(const QJsonValue &value, const QString &expected_type) {
  if (expected_type == "string") {
    return value.isString();
  }
  if (expected_type == "number") {
    return value.isDouble();
  }
  if (expected_type == "integer") {
    return isInteger(value);
  }
  if (expected_type == "null") {
    return value.isNull();
  }
  if (expected_type == "boolean") {
    return value.isBool();
  }
  if (expected_type == "object") {
    return value.isObject();
  }
  if (expected_type == "array") {
    return value.isArray();
  }
  return false;
}

bool validateConstraints(const QString &field, const QJsonValue &value,
                         const QJsonObject &property_schema,
                         QString *error_out) {
  if (property_schema.contains("const") &&
      value != property_schema.value("const")) {
    const QJsonValue expected = property_schema.value("const");
    if (expected.isString()) {
      *error_out = QString("Field '%1' must equal \"%2\"")
                       .arg(field, expected.toString());
    } else if (expected.isDouble()) {
      *error_out =
          QString("Field '%1' must equal %2").arg(field).arg(expected.toDouble());
    } else if (expected.isBool()) {
      *error_out = QString("Field '%1' must equal %2")
                       .arg(field, expected.toBool() ? "true" : "false");
    } else if (expected.isNull()) {
      *error_out = QString("Field '%1' must equal null").arg(field);
    } else {
      *error_out = QString("Field '%1' did not match required constant")
                       .arg(field);
    }
    return false;
  }

  if (property_schema.contains("enum")) {
    const QJsonArray enum_values = property_schema.value("enum").toArray();
    bool found = false;
    for (const QJsonValue &enum_value : enum_values) {
      if (enum_value == value) {
        found = true;
        break;
      }
    }
    if (!found) {
      *error_out = QString("Field '%1' has value outside enum").arg(field);
      return false;
    }
  }

  if ((property_schema.contains("minimum") || property_schema.contains("maximum")) &&
      !value.isNull()) {
    if (!value.isDouble()) {
      *error_out = QString("Field '%1' must be numeric").arg(field);
      return false;
    }
    const double n = value.toDouble();
    if (property_schema.contains("minimum") &&
        n < property_schema.value("minimum").toDouble()) {
      *error_out = QString("Field '%1' is below minimum").arg(field);
      return false;
    }
    if (property_schema.contains("maximum") &&
        n > property_schema.value("maximum").toDouble()) {
      *error_out = QString("Field '%1' is above maximum").arg(field);
      return false;
    }
  }

  return true;
}

bool validateProperty(const QString &field, const QJsonValue &value,
                      const QJsonObject &property_schema, QString *error_out) {
  if (property_schema.contains("oneOf")) {
    const QJsonArray options = property_schema.value("oneOf").toArray();
    for (const QJsonValue &option : options) {
      if (!option.isObject()) {
        continue;
      }
      QString ignored_error;
      if (validateProperty(field, value, option.toObject(), &ignored_error)) {
        return true;
      }
    }
    *error_out = QString("Field '%1' does not match any allowed type").arg(field);
    return false;
  }

  if (property_schema.contains("type")) {
    const QString expected_type = property_schema.value("type").toString();
    if (!validateType(value, expected_type)) {
      *error_out = QString("Field '%1' expected type '%2' but got '%3'")
                       .arg(field, expected_type, describeValueType(value));
      return false;
    }
  }

  return validateConstraints(field, value, property_schema, error_out);
}

void validatePayload(const QJsonObject &obj) {
  if (g_runtime_schema.isEmpty()) {
    throw std::runtime_error("Vitals schema not loaded");
  }

  const QJsonArray required = g_runtime_schema.value("required").toArray();
  for (const QJsonValue &required_field : required) {
    const QString field = required_field.toString();
    if (!obj.contains(field)) {
      throw std::out_of_range(std::string("Missing field: ") +
                              field.toStdString());
    }
  }

  const QJsonObject properties = g_runtime_schema.value("properties").toObject();
  const bool allow_additional_properties =
      g_runtime_schema.value("additionalProperties").toBool(false);
  if (!allow_additional_properties) {
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
      if (!properties.contains(it.key())) {
        throw std::runtime_error(
            QString("Contract violation: unknown field '%1' (schema: %2)")
                .arg(it.key(), g_runtime_schema_path)
                .toStdString());
      }
    }
  }

  for (auto it = properties.constBegin(); it != properties.constEnd(); ++it) {
    if (!obj.contains(it.key())) {
      continue;
    }
    QString error;
    if (!validateProperty(it.key(), obj.value(it.key()), it.value().toObject(),
                          &error)) {
      throw std::runtime_error(
          QString("Contract violation: %1 (schema: %2)")
              .arg(error, g_runtime_schema_path)
              .toStdString());
    }
  }
}
} // namespace

void initializeValidator(const QString &schema_path) {
  QFile schema_file(schema_path);
  if (!schema_file.open(QIODevice::ReadOnly)) {
    throw std::runtime_error(
        QString("Cannot open schema file: %1").arg(schema_path).toStdString());
  }

  const QJsonDocument schema_doc = QJsonDocument::fromJson(schema_file.readAll());
  schema_file.close();
  if (schema_doc.isNull() || !schema_doc.isObject()) {
    throw std::runtime_error(
        QString("Invalid schema JSON: %1").arg(schema_path).toStdString());
  }

  const QJsonObject schema = schema_doc.object();
  if (!schema.contains("required") || !schema.value("required").isArray() ||
      !schema.contains("properties") || !schema.value("properties").isObject()) {
    throw std::runtime_error(
        QString("Schema missing required/properties sections: %1")
            .arg(schema_path)
            .toStdString());
  }

  g_runtime_schema = schema;
  g_runtime_schema_path = schema_path;
}

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
  validatePayload(obj);

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
