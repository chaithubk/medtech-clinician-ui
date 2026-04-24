#pragma once

#include <QObject>

class DashboardWindow : public QObject {
  Q_OBJECT

public:
  explicit DashboardWindow(QObject *parent = nullptr);
};
