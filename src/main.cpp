#include "ui/dashboard_window.h"
#include "utils/logger.h"

#include <QGuiApplication>
#include <csignal>

namespace {
/** @brief Pointer used by the POSIX signal handler to request a clean exit. */
QGuiApplication *g_app = nullptr;

void handleSignal(int sig) {
  if (g_app) {
    Logger::info(
        QString("Received signal %1 — initiating clean shutdown").arg(sig));
    g_app->quit();
  }
}
} // namespace

/**
 * @brief Application entry point.
 *
 * Creates a QGuiApplication, installs signal handlers for graceful shutdown,
 * constructs the DashboardWindow (which starts MQTT and loads QML), then
 * enters the Qt event loop.
 */
int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  g_app = &app;

  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);

  Logger::info("Starting MedTech Clinician UI — Stage 1");

  DashboardWindow window;

  return app.exec();
}
