#pragma once

#include <QString>

class MQTTClient {
public:
  MQTTClient(const QString &broker_host, int broker_port,
             const QString &client_id = QString());

  bool connect();
  bool subscribe(const QString &topic);
  void disconnect();
  bool is_connected() const;

private:
  QString broker_host_;
  QString client_id_;
  int broker_port_;
  bool connected_;
};
