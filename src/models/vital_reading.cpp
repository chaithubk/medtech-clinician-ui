#include "vital_reading.h"

#include <QStringList>

bool VitalReading::isValid() const {
  return hr >= 30.0f && hr <= 180.0f && o2_sat >= 50.0f && o2_sat <= 100.0f;
}

bool VitalReading::isStale(qint64 current_time_ms) const {
  return (current_time_ms - timestamp) > 5000;
}

QString VitalReading::toString() const {
  QStringList parts;
  parts << QString("ts=%1").arg(timestamp) << QString("hr=%1").arg(hr)
        << QString("bp=%1/%2").arg(bp_sys).arg(bp_dia)
        << QString("o2=%1").arg(o2_sat) << QString("temp=%1").arg(temperature)
        << QString("quality=%1").arg(quality)
        << QString("source=%1").arg(source);
  return parts.join(", ");
}
