#pragma once

#include <QString>
#include <QtGlobal>

/**
 * @brief Plain-old-data struct holding one vital-sign measurement.
 *
 * Used throughout the pipeline from MQTT parsing to QML display.
 * All fields are public; no invariants are enforced at the struct level —
 * call isValid() to check whether the values are in physiological range.
 */
struct VitalReading {
  /** Measurement time — milliseconds since Unix epoch. */
  qint64 timestamp = 0;
  /** Heart rate in beats per minute. Normal range: 30–180. */
  float hr = 0.0f;
  /** Systolic blood pressure in mmHg. */
  float bp_sys = 0.0f;
  /** Diastolic blood pressure in mmHg. */
  float bp_dia = 0.0f;
  /** Peripheral oxygen saturation (SpO₂) in percent. Normal range: 50–100. */
  float o2_sat = 0.0f;
  /** Body temperature in degrees Celsius. */
  float temperature = 0.0f;
  /** Data quality score 0–100 as reported by the sensor firmware. */
  int quality = 0;
  /** Data source label, e.g. "simulator" or "bedside-monitor". */
  QString source;

  /** @brief Return true when all vital values are within physiological range.
   */
  bool isValid() const;

  /**
   * @brief Return true when the reading is older than 5 seconds.
   * @param current_time_ms Current wall-clock time in ms since epoch.
   */
  bool isStale(qint64 current_time_ms) const;

  /** @brief One-line debug representation of all fields. */
  QString toString() const;
};
