#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

struct mosquitto;

/**
 * @brief MQTT client wrapper using libmosquitto.
 *
 * Connects to an MQTT broker, subscribes to topics, and emits Qt signals when
 * messages are received or the connection state changes. Implements exponential
 * backoff reconnection (1 s → 2 s → 4 s … max 60 s).
 */
class MQTTClient : public QObject {
  Q_OBJECT

public:
  /**
   * @brief Construct an MQTTClient.
   * @param broker_host Hostname or IP of the MQTT broker.
   * @param broker_port TCP port of the MQTT broker (default 1883).
   * @param client_id   MQTT client identifier (empty = auto-generated).
   * @param parent      Optional Qt parent object.
   */
  explicit MQTTClient(const QString &broker_host, int broker_port,
                      const QString &client_id = QString(),
                      QObject *parent = nullptr);

  /** @brief Destructor — stops the mosquitto loop and frees resources. */
  ~MQTTClient() override;

  /** @brief Connect to the MQTT broker. Returns false on immediate failure. */
  bool connect();

  /** @brief Subscribe to @p topic at QoS 1. Returns false on failure. */
  bool subscribe(const QString &topic);

  /** @brief Gracefully close the MQTT connection (no auto-reconnect). */
  void disconnect();

  /** @brief Return true if currently connected to the broker. */
  bool is_connected() const;

signals:
  /** @brief Emitted when a message arrives on a subscribed topic. */
  void messageReceived(const QString &topic, const QString &payload);

  /** @brief Emitted when the connection to the broker is established. */
  void connected();

  /** @brief Emitted when the connection to the broker is lost. */
  void disconnected();

  /** @brief Emitted when an MQTT-level error occurs. */
  void error(const QString &error_message);

private slots:
  /** @brief Slot invoked by the reconnect timer to attempt reconnection. */
  void attemptReconnect();

private:
  // Static C-API callback wrappers (libmosquitto requirement)
  static void s_on_connect(struct mosquitto *mosq, void *obj, int rc);
  static void s_on_disconnect(struct mosquitto *mosq, void *obj, int rc);
  static void s_on_message(struct mosquitto *mosq, void *obj,
                           const struct mosquitto_message *msg);

  /** @brief Called on the Qt main thread when the broker accepts the
   * connection. */
  void on_connect_callback(int rc);

  /** @brief Called on the Qt main thread when the broker closes the
   * connection. */
  void on_disconnect_callback(int rc);

  /** @brief Called on the Qt main thread when a subscribed message arrives. */
  void on_message_callback(const QString &topic, const QString &payload);

  /** @brief Schedule the next reconnection attempt with exponential backoff. */
  void scheduleReconnect();

  struct mosquitto *m_mosq{nullptr};
  QString m_broker_host;
  QString m_client_id;
  int m_broker_port;
  bool m_connected{false};
  bool m_intentional_disconnect{false};

  QTimer *m_reconnect_timer;
  int m_reconnect_delay_ms{1000};
  static constexpr int k_max_reconnect_delay_ms = 60000;
};
