#pragma once

#include <QString>
#include <QtGlobal>

struct VitalReading {
  qint64 timestamp = 0;
  float hr = 0.0f;
  float bp_sys = 0.0f;
  float bp_dia = 0.0f;
  float o2_sat = 0.0f;
  float temperature = 0.0f;
  int quality = 0;
  QString source;

  bool isValid() const;
  bool isStale(qint64 current_time_ms) const;
  QString toString() const;
};
