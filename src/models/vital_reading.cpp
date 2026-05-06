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
  parts << QString("version=%1").arg(version)
        << QString("patient_id=%1").arg(patient_id)
        << QString("scenario=%1/%2").arg(scenario, scenario_stage)
        << QString("ts=%1").arg(timestamp) << QString("hr=%1").arg(hr)
        << QString("bp=%1/%2").arg(bp_sys).arg(bp_dia)
        << QString("o2=%1").arg(o2_sat) << QString("temp=%1").arg(temperature)
        << QString("rr=%1").arg(respiratory_rate) << QString("wbc=%1").arg(wbc)
        << QString("lactate=%1").arg(lactate)
        << QString("sirs=%1").arg(sirs_score)
        << QString("qsofa=%1").arg(qsofa_score)
        << QString("sepsis_stage=%1").arg(sepsis_stage)
        << QString("sepsis_onset_ts=%1")
               .arg(sepsis_onset_ts.has_value()
                        ? QString::number(*sepsis_onset_ts)
                        : QString("null"))
        << QString("quality=%1").arg(quality)
        << QString("source=%1").arg(source);
  return parts.join(", ");
}
