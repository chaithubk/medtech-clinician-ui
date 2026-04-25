#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

class MQTTClient;
class UIModel;
class QQmlApplicationEngine;

/**
 * @brief Orchestrates the MQTT client, UIModel, and QML engine.
 *
 * Creates all subsystems, connects signals/slots, and exposes the UIModel to
 * QML via a context property named "uiModel".
 */
class DashboardWindow : public QObject {
  Q_OBJECT

public:
  /** @brief Construct and start the dashboard (MQTT connect + QML load). */
  explicit DashboardWindow(QObject *parent = nullptr);

  ~DashboardWindow() override;

public slots:
  /** @brief Invoked when the MQTT broker accepts the connection. */
  void onMQTTConnected();

  /** @brief Invoked when the MQTT connection is lost. */
  void onMQTTDisconnected();

  /**
   * @brief Invoked when a message arrives on a subscribed topic.
   * @param topic   MQTT topic string.
   * @param payload Raw JSON payload.
   */
  void onMQTTMessageReceived(const QString &topic, const QString &payload);

  /** @brief Called by the status timer to detect stale vital data. */
  void onTimeout();

private:
  MQTTClient *m_mqtt_client{nullptr};
  UIModel *m_ui_model{nullptr};
  QQmlApplicationEngine *m_engine{nullptr};
  QTimer *m_status_timer{nullptr};
  int m_heartbeat_tick{0};
};
