#pragma once

#include "vital_reading.h"
#include <QObject>
#include <QString>

/**
 * @brief Qt model exposing vital-sign data to QML via Q_PROPERTY bindings.
 *
 * All getter methods are bound to QML properties.  Call setVital() /
 * setStatus() from C++ and the QML layer will update automatically through the
 * signals.
 */
class UIModel : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString status READ getStatus NOTIFY statusChanged)
  Q_PROPERTY(QString hr_value READ getHrValue NOTIFY vitalsUpdated)
  Q_PROPERTY(QString bp_value READ getBpValue NOTIFY vitalsUpdated)
  Q_PROPERTY(QString o2_value READ getO2Value NOTIFY vitalsUpdated)
  Q_PROPERTY(QString temp_value READ getTempValue NOTIFY vitalsUpdated)
  Q_PROPERTY(QString quality_value READ getQualityValue NOTIFY vitalsUpdated)
  Q_PROPERTY(QString last_update READ getLastUpdate NOTIFY vitalsUpdated)

public:
  explicit UIModel(QObject *parent = nullptr);

  /** @brief Store a new vital reading and notify QML. */
  void setVital(const VitalReading &vital);

  /** @brief Return the most recently stored VitalReading. */
  VitalReading getCurrentVital() const;

  /** @brief Update the connection/data status string. */
  void setStatus(const QString &status);

  // Property getters
  QString getStatus() const;
  QString getHrValue() const;
  QString getBpValue() const;
  QString getO2Value() const;
  QString getTempValue() const;
  QString getQualityValue() const;

  /** @brief Human-readable time elapsed since the last vital update. */
  QString getLastUpdate() const;

signals:
  /** @brief Emitted whenever a new VitalReading is stored. */
  void vitalsUpdated();

  /** @brief Emitted when the status string changes. */
  void statusChanged(const QString &new_status);

private:
  VitalReading m_current_vital;
  QString m_status{"No Data"};
};
