#include "config.h"

#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>

namespace {
int envToInt(const QString &key, int default_value) {
  bool ok = false;
  const int value =
      QProcessEnvironment::systemEnvironment().value(key).toInt(&ok);
  return ok ? value : default_value;
}

const QString kInstalledContractPinPath =
    "/usr/share/medtech/contracts/contract-pin.json";
const QString kLocalContractPinPath = "contracts/contract-pin.json";
const QString kInstalledSchemaPath =
    "/usr/share/medtech/contracts/schemas/vitals/vitals.schema.json";
const QString kLocalSchemaPath = "contracts/schemas/vitals/vitals.schema.json";

QString resolveDefaultContractPinPath() {
  if (QFile::exists(kInstalledContractPinPath)) {
    return kInstalledContractPinPath;
  }
  if (QFile::exists(kLocalContractPinPath)) {
    return kLocalContractPinPath;
  }
  return kInstalledContractPinPath;
}

QString firstExistingPath(const QString &first, const QString &second,
                          const QString &fallback) {
  if (!first.isEmpty() && QFile::exists(first)) {
    return first;
  }
  if (!second.isEmpty() && QFile::exists(second)) {
    return second;
  }
  if (QFile::exists(fallback)) {
    return fallback;
  }
  return fallback;
}

QString resolveSchemaPathFromPin(const QString &pin_path) {
  static const QString kFallbackPath = kInstalledSchemaPath;

  QFile pin_file(pin_path);
  if (!pin_file.open(QIODevice::ReadOnly)) {
    return firstExistingPath(QString(), kLocalSchemaPath, kFallbackPath);
  }

  const QJsonDocument pin_doc = QJsonDocument::fromJson(pin_file.readAll());
  pin_file.close();
  if (pin_doc.isNull() || !pin_doc.isObject()) {
    return firstExistingPath(QString(), kLocalSchemaPath, kFallbackPath);
  }

  const QJsonObject consumer = pin_doc.object().value("consumer").toObject();
  const QString runtime_schema_path =
      consumer.value("runtime_schema_path").toString();
  const QString vendored_schema_path =
      consumer.value("vendored_schema_path").toString();
  return firstExistingPath(
      runtime_schema_path, vendored_schema_path,
      firstExistingPath(QString(), kLocalSchemaPath, kFallbackPath));
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
const QString CONTRACT_PIN_PATH =
    getEnv("MEDTECH_CONTRACT_PIN", resolveDefaultContractPinPath());
const QString VITALS_SCHEMA_PATH = getEnv(
    "MEDTECH_VITALS_SCHEMA", resolveSchemaPathFromPin(CONTRACT_PIN_PATH));
const int WINDOW_WIDTH = envToInt("WINDOW_WIDTH", 1920);
const int WINDOW_HEIGHT = envToInt("WINDOW_HEIGHT", 1080);
} // namespace Config
