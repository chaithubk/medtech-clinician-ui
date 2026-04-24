#include "mqtt_client.h"
#include "utils/logger.h"

#include <QMetaObject>
#include <mosquitto.h>

MQTTClient::MQTTClient(const QString &broker_host, int broker_port,
                       const QString &client_id, QObject *parent)
    : QObject(parent), m_broker_host(broker_host), m_client_id(client_id),
      m_broker_port(broker_port) {
  mosquitto_lib_init();

  const QByteArray id = client_id.isEmpty() ? QByteArray() : client_id.toUtf8();
  m_mosq = mosquitto_new(id.isEmpty() ? nullptr : id.constData(), true, this);

  if (m_mosq) {
    mosquitto_connect_callback_set(m_mosq, s_on_connect);
    mosquitto_disconnect_callback_set(m_mosq, s_on_disconnect);
    mosquitto_message_callback_set(m_mosq, s_on_message);
  }

  m_reconnect_timer = new QTimer(this);
  m_reconnect_timer->setSingleShot(true);
  QObject::connect(m_reconnect_timer, &QTimer::timeout, this,
                   &MQTTClient::attemptReconnect);
}

MQTTClient::~MQTTClient() {
  m_reconnect_timer->stop();
  if (m_mosq) {
    mosquitto_disconnect(m_mosq);
    mosquitto_loop_stop(m_mosq, true);
    mosquitto_destroy(m_mosq);
    m_mosq = nullptr;
  }
  mosquitto_lib_cleanup();
}

bool MQTTClient::connect() {
  if (!m_mosq) {
    emit error("Failed to create mosquitto instance");
    return false;
  }

  m_intentional_disconnect = false;
  mosquitto_loop_start(m_mosq);

  const int rc =
      mosquitto_connect_async(m_mosq, m_broker_host.toUtf8().constData(),
                              m_broker_port, /* keepalive */ 60);
  if (rc != MOSQ_ERR_SUCCESS) {
    Logger::warn(
        QString("MQTT connect_async failed: %1").arg(mosquitto_strerror(rc)));
    scheduleReconnect();
    return false;
  }
  return true;
}

bool MQTTClient::subscribe(const QString &topic) {
  if (!m_mosq || !m_connected) {
    return false;
  }
  const int rc =
      mosquitto_subscribe(m_mosq, nullptr, topic.toUtf8().constData(), 1);
  return rc == MOSQ_ERR_SUCCESS;
}

void MQTTClient::disconnect() {
  m_intentional_disconnect = true;
  m_reconnect_timer->stop();
  if (m_mosq) {
    mosquitto_disconnect(m_mosq);
  }
}

bool MQTTClient::is_connected() const { return m_connected; }

void MQTTClient::attemptReconnect() {
  Logger::info(QString("Attempting MQTT reconnect to %1:%2")
                   .arg(m_broker_host)
                   .arg(m_broker_port));
  if (!m_mosq) {
    return;
  }
  const int rc = mosquitto_reconnect_async(m_mosq);
  if (rc != MOSQ_ERR_SUCCESS) {
    Logger::warn(
        QString("MQTT reconnect failed: %1").arg(mosquitto_strerror(rc)));
    scheduleReconnect();
  }
}

void MQTTClient::scheduleReconnect() {
  Logger::info(
      QString("Scheduling MQTT reconnect in %1 ms").arg(m_reconnect_delay_ms));
  m_reconnect_timer->start(m_reconnect_delay_ms);
  m_reconnect_delay_ms =
      qMin(m_reconnect_delay_ms * 2, k_max_reconnect_delay_ms);
}

// ---------------------------------------------------------------------------
// Static C-API callback wrappers
// ---------------------------------------------------------------------------

void MQTTClient::s_on_connect(struct mosquitto * /*mosq*/, void *obj, int rc) {
  auto *self = static_cast<MQTTClient *>(obj);
  QMetaObject::invokeMethod(
      self, [self, rc]() { self->on_connect_callback(rc); },
      Qt::QueuedConnection);
}

void MQTTClient::s_on_disconnect(struct mosquitto * /*mosq*/, void *obj,
                                 int rc) {
  auto *self = static_cast<MQTTClient *>(obj);
  QMetaObject::invokeMethod(
      self, [self, rc]() { self->on_disconnect_callback(rc); },
      Qt::QueuedConnection);
}

void MQTTClient::s_on_message(struct mosquitto * /*mosq*/, void *obj,
                              const struct mosquitto_message *msg) {
  auto *self = static_cast<MQTTClient *>(obj);
  // Copy data before the message struct is freed by libmosquitto
  QString topic = QString::fromUtf8(msg->topic);
  QString payload = QString::fromUtf8(static_cast<const char *>(msg->payload),
                                      msg->payloadlen);
  QMetaObject::invokeMethod(
      self,
      [self, topic, payload]() { self->on_message_callback(topic, payload); },
      Qt::QueuedConnection);
}

// ---------------------------------------------------------------------------
// Instance-level callback implementations (called on the Qt main thread)
// ---------------------------------------------------------------------------

void MQTTClient::on_connect_callback(int rc) {
  if (rc == 0) {
    m_connected = true;
    m_reconnect_delay_ms = 1000; // Reset backoff on successful connect
    Logger::info("MQTT connected to broker");
    emit connected();
  } else {
    Logger::warn(QString("MQTT connection refused (code %1)").arg(rc));
    emit error(QString("Connection refused (code %1)").arg(rc));
    scheduleReconnect();
  }
}

void MQTTClient::on_disconnect_callback(int rc) {
  m_connected = false;
  Logger::info("MQTT disconnected from broker");
  emit disconnected();

  if (!m_intentional_disconnect && rc != 0) {
    scheduleReconnect();
  }
  m_intentional_disconnect = false;
}

void MQTTClient::on_message_callback(const QString &topic,
                                     const QString &payload) {
  emit messageReceived(topic, payload);
}
