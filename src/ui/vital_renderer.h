#pragma once

#include <QString>

class VitalRenderer {
public:
  static QString formatHR(float hr);
  static QString formatBP(float sys, float dia);
  static QString formatO2(float o2);
  static QString formatTemp(float temp);
  static QString formatQuality(int quality);
  static QString getStatusColor(const QString &status);
};
