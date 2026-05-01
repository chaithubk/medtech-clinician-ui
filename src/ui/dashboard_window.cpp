#include "dashboard_window.h"
#include "models/ui_model.h"
#include "mqtt/mqtt_client.h"
#include "mqtt/mqtt_payload.h"
#include "utils/config.h"
#include "utils/logger.h"

#include <QDateTime>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <stdexcept>

DashboardWindow::DashboardWindow(QObject *parent) : QObject(parent) {
  // Create the data model first so QML can bind to it
  m_ui_model = new UIModel(this);
  m_ui_model->setStatus("Connecting...");

  // Set up the QML engine and expose the model
  m_engine = new QQmlApplicationEngine(this);
  m_engine->rootContext()->setContextProperty("uiModel", m_ui_model);
  m_engine->load(QUrl("qrc:/qml/main.qml"));

  // Create the MQTT client
  m_mqtt_client =
      new MQTTClient(Config::MQTT_BROKER, Config::MQTT_PORT, QString(), this);

  // Connect MQTT signals to window slots
  QObject::connect(m_mqtt_client, &MQTTClient::connected, this,
                   &DashboardWindow::onMQTTConnected);
  QObject::connect(m_mqtt_client, &MQTTClient::disconnected, this,
                   &DashboardWindow::onMQTTDisconnected);
  QObject::connect(m_mqtt_client, &MQTTClient::messageReceived, this,
                   &DashboardWindow::onMQTTMessageReceived);
  QObject::connect(
      m_mqtt_client, &MQTTClient::error, this,
      [](const QString &msg) { Logger::error("MQTT error: " + msg); });

  // Periodic timer to detect stale vital data
  m_status_timer = new QTimer(this);
  QObject::connect(m_status_timer, &QTimer::timeout, this,
                   &DashboardWindow::onTimeout);
  m_status_timer->start(Config::STATUS_UPDATE_INTERVAL_MS);

  m_mqtt_client->connect();
}

DashboardWindow::~DashboardWindow() = default;

void DashboardWindow::onMQTTConnected() {
  Logger::info("MQTT connected — subscribing to vitals topic");
  m_ui_model->setStatus("Connected");
  m_mqtt_client->subscribe(Config::MQTT_TOPIC_VITALS);
}

void DashboardWindow::onMQTTDisconnected() {
  Logger::warn("MQTT disconnected");
  m_ui_model->setStatus("Connecting...");
}

void DashboardWindow::onMQTTMessageReceived(const QString &topic,
                                            const QString &payload) {
  Logger::info(QString("Payload received on '%1': %2").arg(topic, payload));
  try {
    const VitalReading reading = MqttPayload::parseVital(payload);
    m_ui_model->setVital(reading);
    Logger::info(
        QString("UI model updated — HR: %1  BP: %2  SpO2: %3  Temp: %4  "
                "Quality: %5")
            .arg(m_ui_model->getHrValue(), m_ui_model->getBpValue(),
                 m_ui_model->getO2Value(), m_ui_model->getTempValue(),
                 m_ui_model->getQualityValue()));
    if (m_ui_model->getStatus() == "Stale Data") {
      m_ui_model->setStatus("Connected");
    }
  } catch (const std::exception &e) {
    Logger::error(QString("Failed to parse vital payload: %1").arg(e.what()));
    m_ui_model->setStatus("No Data");
  }
}

void DashboardWindow::onTimeout() {
  // Heartbeat log every 30 s so the terminal confirms the process is alive
  ++m_heartbeat_tick;
  if (m_heartbeat_tick % 30 == 0) {
    Logger::info(
        QString("Dashboard running — status: %1, last update: %2")
            .arg(m_ui_model->getStatus(), m_ui_model->getLastUpdate()));
  }

  const VitalReading vital = m_ui_model->getCurrentVital();
  if (vital.timestamp == 0) {
    return; // No data received yet
  }

  const qint64 now_ms = QDateTime::currentMSecsSinceEpoch();
  if (vital.isStale(now_ms) && (m_ui_model->getStatus() == "Connected" ||
                                m_ui_model->getStatus() == "No Data")) {
    Logger::warn("Vital data is stale (>5 s without update)");
    m_ui_model->setStatus("Stale Data");
  }
}
