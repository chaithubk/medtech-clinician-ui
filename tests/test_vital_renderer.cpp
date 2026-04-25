/**
 * @file test_vital_renderer.cpp
 * @brief Additional VitalRenderer formatting tests compiled into the test
 * binary.
 *
 * This translation unit is included in the test_mqtt_client executable.
 * Declare run_vital_renderer_tests() and call it from test_mqtt_client.cpp's
 * main() if additional tests are needed; for now the tests live in
 * test_mqtt_client.cpp for simplicity.
 */

#include "../src/ui/vital_renderer.h"

/** @brief Ensures the VitalRenderer translation unit is linked correctly. */
void vital_renderer_link_anchor() {
  (void)VitalRenderer::formatBP(120.0f, 80.0f);
  (void)VitalRenderer::formatO2(98.0f);
  (void)VitalRenderer::formatTemp(37.2f);
  (void)VitalRenderer::formatQuality(95);
  (void)VitalRenderer::getStatusColor("Connected");
  (void)VitalRenderer::getStatusColor("Stale Data");
  (void)VitalRenderer::getStatusColor("No Data");
}
