#pragma once

#include "vital_reading.h"
#include <QObject>
#include <QString>

class UIModel : public QObject {
  Q_OBJECT

public:
  explicit UIModel(QObject *parent = nullptr);

  void setVital(const VitalReading &vital);
  VitalReading getCurrentVital() const;
  void setStatus(const QString &status);
  QString getStatus() const;

signals:
  void vitalsUpdated();
  void statusChanged(const QString &new_status);

private:
  VitalReading current_vital_;
  QString status_;
};
