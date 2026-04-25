#pragma once

#include <QString>

/**
 * @brief Pure utility class for formatting vital-sign values as display
 * strings.
 *
 * All methods are static; no state is held.
 */
class VitalRenderer {
public:
  /** @brief Format heart rate. Example: formatHR(92.0f) → "92 bpm" */
  static QString formatHR(float hr);

  /** @brief Format blood pressure. Example: formatBP(135, 85) → "135/85 mmHg"
   */
  static QString formatBP(float sys, float dia);

  /** @brief Format oxygen saturation. Example: formatO2(98.0f) → "98 %" */
  static QString formatO2(float o2);

  /** @brief Format temperature. Example: formatTemp(37.2f) → "37.2 °C" */
  static QString formatTemp(float temp);

  /** @brief Format data quality score. Example: formatQuality(95) → "95" */
  static QString formatQuality(int quality);

  /**
   * @brief Return a hex colour string for the given status label.
   * @return "#00AA00" (Connected), "#FFAA00" (Connecting/Stale), "#AA0000"
   * (other)
   */
  static QString getStatusColor(const QString &status);
};
