#include "ui_model.h"
#include "ui/vital_renderer.h"

#include <QDateTime>

UIModel::UIModel(QObject *parent) : QObject(parent) {}

void UIModel::setVital(const VitalReading &vital) {
  m_current_vital = vital;
  emit vitalsUpdated();
}

VitalReading UIModel::getCurrentVital() const { return m_current_vital; }

void UIModel::setStatus(const QString &status) {
  if (m_status == status) {
    return;
  }
  m_status = status;
  emit statusChanged(m_status);
}

QString UIModel::getStatus() const { return m_status; }

QString UIModel::getHrValue() const {
  return VitalRenderer::formatHR(m_current_vital.hr);
}

QString UIModel::getBpValue() const {
  return VitalRenderer::formatBP(m_current_vital.bp_sys,
                                 m_current_vital.bp_dia);
}

QString UIModel::getO2Value() const {
  return VitalRenderer::formatO2(m_current_vital.o2_sat);
}

QString UIModel::getTempValue() const {
  return VitalRenderer::formatTemp(m_current_vital.temperature);
}

QString UIModel::getQualityValue() const {
  return VitalRenderer::formatQuality(m_current_vital.quality);
}

QString UIModel::getLastUpdate() const {
  if (m_current_vital.timestamp == 0) {
    return "--";
  }
  const qint64 now_ms = QDateTime::currentMSecsSinceEpoch();
  const qint64 diff_s = (now_ms - m_current_vital.timestamp) / 1000;
  return QString("%1s ago").arg(diff_s);
}
