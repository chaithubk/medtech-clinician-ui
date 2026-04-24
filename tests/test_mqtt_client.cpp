/**
 * @file test_mqtt_client.cpp
 * @brief Unit tests for MqttPayload, VitalReading, UIModel, and VitalRenderer.
 *
 * Simple custom test runner — no external frameworks required.
 * Exit code 0 = all tests passed.  Exit code 1 = at least one failure.
 */

#include "models/ui_model.h"
#include "models/vital_reading.h"
#include "mqtt/mqtt_payload.h"
#include "test_fixtures.h"
#include "ui/vital_renderer.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QString>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Minimal test harness
// ---------------------------------------------------------------------------
static int g_pass = 0;
static int g_fail = 0;

#define ASSERT_TRUE(expr)                                                      \
  do {                                                                         \
    if (!(expr)) {                                                             \
      std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__                    \
                << "  assertion failed: " #expr "\n";                          \
      ++g_fail;                                                                \
    } else {                                                                   \
      ++g_pass;                                                                \
    }                                                                          \
  } while (false)

#define ASSERT_EQ(a, b)                                                        \
  do {                                                                         \
    if ((a) != (b)) {                                                          \
      std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__                    \
                << "  ASSERT_EQ failed: (" #a ") != (" #b ")\n";               \
      ++g_fail;                                                                \
    } else {                                                                   \
      ++g_pass;                                                                \
    }                                                                          \
  } while (false)

#define ASSERT_THROWS(expr, ExcType)                                           \
  do {                                                                         \
    bool _threw = false;                                                       \
    try {                                                                      \
      (void)(expr);                                                            \
    } catch (const ExcType &) {                                                \
      _threw = true;                                                           \
    } catch (...) {                                                            \
    }                                                                          \
    if (!_threw) {                                                             \
      std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__                    \
                << "  expected exception " #ExcType " not thrown\n";           \
      ++g_fail;                                                                \
    } else {                                                                   \
      ++g_pass;                                                                \
    }                                                                          \
  } while (false)

// ---------------------------------------------------------------------------
// Test 1: parseVital with valid JSON → VitalReading correct
// ---------------------------------------------------------------------------
static void test_parseVital_valid() {
  const QString json = R"({"timestamp":1712973600000,"hr":92.0,"bp_sys":135.0,)"
                       R"("bp_dia":85.0,"o2_sat":98.0,"temperature":37.2,)"
                       R"("quality":95,"source":"simulator"})";

  VitalReading r = MqttPayload::parseVital(json);
  ASSERT_EQ(r.timestamp, static_cast<qint64>(1712973600000LL));
  ASSERT_TRUE(qAbs(r.hr - 92.0f) < 0.01f);
  ASSERT_TRUE(qAbs(r.bp_sys - 135.0f) < 0.01f);
  ASSERT_TRUE(qAbs(r.bp_dia - 85.0f) < 0.01f);
  ASSERT_TRUE(qAbs(r.o2_sat - 98.0f) < 0.01f);
  ASSERT_TRUE(qAbs(r.temperature - 37.2f) < 0.01f);
  ASSERT_EQ(r.quality, 95);
  ASSERT_EQ(r.source, QString("simulator"));
}

// ---------------------------------------------------------------------------
// Test 2: parseVital with invalid JSON → throws std::runtime_error
// ---------------------------------------------------------------------------
static void test_parseVital_invalid_json() {
  ASSERT_THROWS(MqttPayload::parseVital("{not valid json!!!}"),
                std::runtime_error);
  ASSERT_THROWS(MqttPayload::parseVital(""), std::runtime_error);
}

// ---------------------------------------------------------------------------
// Test 3: VitalReading::isValid with values in normal range → true
// ---------------------------------------------------------------------------
static void test_vitalReading_isValid_true() {
  VitalReading r;
  r.hr = 75.0f;
  r.o2_sat = 98.0f;
  ASSERT_TRUE(r.isValid());
}

// ---------------------------------------------------------------------------
// Test 4: VitalReading::isValid with out-of-range HR → false
// ---------------------------------------------------------------------------
static void test_vitalReading_isValid_false() {
  VitalReading r;
  r.hr = 10.0f; // below 30 bpm
  r.o2_sat = 98.0f;
  ASSERT_TRUE(!r.isValid());

  r.hr = 250.0f; // above 180 bpm
  ASSERT_TRUE(!r.isValid());
}

// ---------------------------------------------------------------------------
// Test 5: VitalReading::isStale with recent timestamp → false
// ---------------------------------------------------------------------------
static void test_vitalReading_isStale_false() {
  VitalReading r;
  r.timestamp = QDateTime::currentMSecsSinceEpoch();
  ASSERT_TRUE(!r.isStale(QDateTime::currentMSecsSinceEpoch()));
}

// ---------------------------------------------------------------------------
// Test 6: VitalReading::isStale with >5 s old timestamp → true
// ---------------------------------------------------------------------------
static void test_vitalReading_isStale_true() {
  VitalReading r;
  r.timestamp = QDateTime::currentMSecsSinceEpoch() - 6000; // 6 seconds ago
  ASSERT_TRUE(r.isStale(QDateTime::currentMSecsSinceEpoch()));
}

// ---------------------------------------------------------------------------
// Test 7: VitalRenderer::formatHR
// ---------------------------------------------------------------------------
static void test_formatHR() {
  ASSERT_EQ(VitalRenderer::formatHR(92.0f), QString("92 bpm"));
  ASSERT_EQ(VitalRenderer::formatHR(120.5f), QString("121 bpm"));
}

// ---------------------------------------------------------------------------
// Test 8: VitalRenderer::formatBP
// ---------------------------------------------------------------------------
static void test_formatBP() {
  ASSERT_EQ(VitalRenderer::formatBP(135.0f, 85.0f), QString("135/85 mmHg"));
  ASSERT_EQ(VitalRenderer::formatBP(120.0f, 80.0f), QString("120/80 mmHg"));
}

// ---------------------------------------------------------------------------
// Test 9: Parse valid JSON vital and verify UIModel is updated
// ---------------------------------------------------------------------------
static void test_uiModel_setVital() {
  UIModel model;

  const QString json = R"({"timestamp":1712973600000,"hr":65.0,"bp_sys":120.0,)"
                       R"("bp_dia":80.0,"o2_sat":99.0,"temperature":36.6,)"
                       R"("quality":80,"source":"test"})";

  VitalReading r = MqttPayload::parseVital(json);
  model.setVital(r);

  // Verify formatted strings match expectations
  ASSERT_EQ(model.getHrValue(), QString("65 bpm"));
  ASSERT_EQ(model.getBpValue(), QString("120/80 mmHg"));
  ASSERT_EQ(model.getO2Value(), QString("99 %"));
  ASSERT_EQ(model.getQualityValue(), QString("80"));
}

// ---------------------------------------------------------------------------
// Test 10: UIModel status changes propagate correctly
// ---------------------------------------------------------------------------
static void test_uiModel_status() {
  UIModel model;
  ASSERT_EQ(model.getStatus(), QString("No Data"));

  model.setStatus("Connected");
  ASSERT_EQ(model.getStatus(), QString("Connected"));

  // Setting the same status again should be a no-op (no signal, same value)
  model.setStatus("Connected");
  ASSERT_EQ(model.getStatus(), QString("Connected"));

  model.setStatus("Connecting...");
  ASSERT_EQ(model.getStatus(), QString("Connecting..."));

  model.setStatus("Stale Data");
  ASSERT_EQ(model.getStatus(), QString("Stale Data"));
}

// ---------------------------------------------------------------------------
// Test for missing required field → throws std::out_of_range
// ---------------------------------------------------------------------------
static void test_parseVital_missing_field() {
  // Missing "hr" field
  const QString json = R"({"timestamp":1712973600000,"bp_sys":135.0,)"
                       R"("bp_dia":85.0,"o2_sat":98.0,"temperature":37.2,)"
                       R"("quality":95,"source":"simulator"})";
  ASSERT_THROWS(MqttPayload::parseVital(json), std::out_of_range);
}

// ---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  // QCoreApplication required for QObject, signals, and Qt containers
  QCoreApplication app(argc, argv);

  std::cout << "Running MedTech Stage 1 unit tests...\n";

  test_parseVital_valid();
  test_parseVital_invalid_json();
  test_parseVital_missing_field();
  test_vitalReading_isValid_true();
  test_vitalReading_isValid_false();
  test_vitalReading_isStale_false();
  test_vitalReading_isStale_true();
  test_formatHR();
  test_formatBP();
  test_uiModel_setVital();
  test_uiModel_status();

  std::cout << g_pass << " tests passed, " << g_fail << " tests failed.\n";

  if (g_fail > 0) {
    return EXIT_FAILURE;
  }
  std::cout << "All tests passed ✅\n";
  return EXIT_SUCCESS;
}
