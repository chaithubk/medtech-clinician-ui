#pragma once

#include <QString>
#include <QtGlobal>
#include <optional>

/**
 * @brief Plain-old-data struct holding one vital-sign measurement.
 *
 * Matches the MedTech Vitals Telemetry Contract schema.
 * See contracts/schemas/vitals/vitals.schema.json for the authoritative field
 * definitions.
 *
 * All fields are public; no invariants are enforced at the struct level —
 * call isValid() to check whether the values are in physiological range.
 */
struct VitalReading {
  /** Contract version string in semver format (e.g. "2.1.1"). */
  QString version;
  /** Unique patient or simulated-patient record identifier. */
  QString patient_id;
  /** Clinical scenario: "healthy" | "sepsis" | "critical". */
  QString scenario;
  /** Stage within the scenario: "healthy" | "pre_sepsis" | "sepsis_onset" |
   * "sepsis" | "septic_shock". */
  QString scenario_stage;
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
  /** Respiratory rate in breaths per minute. */
  float respiratory_rate = 0.0f;
  /** White blood cell count in 10³/µL. */
  float wbc = 0.0f;
  /** Blood lactate level in mmol/L. */
  float lactate = 0.0f;
  /** SIRS score (0–4). */
  int sirs_score = 0;
  /** qSOFA score (0–3). */
  int qsofa_score = 0;
  /** Sepsis classification: "none" | "sirs" | "sepsis" | "septic_shock". */
  QString sepsis_stage;
  /** Timestamp (ms epoch) when sepsis onset was detected, or nullopt if not
   * yet detected. */
  std::optional<qint64> sepsis_onset_ts;
  /** Signal quality indicator, e.g. "good", "degraded", "poor". */
  QString quality;
  /** Data source label, e.g. "synthea-simulator" or "bedside-monitor-A". */
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
