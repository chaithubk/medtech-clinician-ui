#include "mqtt_client.h"

MQTTClient::MQTTClient(const QString &broker_host, int broker_port,
                       const QString &client_id)
    : broker_host_(broker_host), client_id_(client_id),
      broker_port_(broker_port), connected_(false) {}

bool MQTTClient::connect() {
  connected_ = true;
  return true;
}

bool MQTTClient::subscribe(const QString &topic) {
  (void)topic;
  return connected_;
}

void MQTTClient::disconnect() { connected_ = false; }

bool MQTTClient::is_connected() const { return connected_; }
