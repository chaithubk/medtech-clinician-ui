#include "ui_model.h"

UIModel::UIModel(QObject *parent) : QObject(parent), status_("No Data") {}

void UIModel::setVital(const VitalReading &vital) {
  current_vital_ = vital;
  emit vitalsUpdated();
}

VitalReading UIModel::getCurrentVital() const { return current_vital_; }

void UIModel::setStatus(const QString &status) {
  if (status_ == status) {
    return;
  }
  status_ = status;
  emit statusChanged(status_);
}

QString UIModel::getStatus() const { return status_; }
