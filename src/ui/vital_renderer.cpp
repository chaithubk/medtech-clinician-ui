#include "vital_renderer.h"

QString VitalRenderer::formatHR(float hr) {
  return QString::number(hr, 'f', 0) + " bpm";
}

QString VitalRenderer::formatBP(float sys, float dia) {
  return QString::number(sys, 'f', 0) + "/" + QString::number(dia, 'f', 0) +
         " mmHg";
}

QString VitalRenderer::formatO2(float o2) {
  return QString::number(o2, 'f', 0) + " %";
}

QString VitalRenderer::formatTemp(float temp) {
  return QString::number(temp, 'f', 1) + " \u00B0C";
}

QString VitalRenderer::formatQuality(int quality) {
  return QString::number(quality);
}

QString VitalRenderer::getStatusColor(const QString &status) {
  if (status == "Connected") {
    return "#00AA00";
  }
  if (status == "Connecting..." || status == "Stale Data") {
    return "#FFAA00";
  }
  return "#AA0000";
}
