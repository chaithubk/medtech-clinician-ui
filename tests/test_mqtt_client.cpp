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
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Minimal test harness
// ---------------------------------------------------------------------------
static int g_pass = 0;
static int g_fail = 0;
static const QString kTestSchemaPath =
    QString(MEDTECH_SOURCE_DIR) + "/contracts/vitals/v2.0.json";

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
// Shared v2 payload builder — produces a minimal valid v2.0 payload.
// Callers may omit a field or override values to test specific scenarios.
// ---------------------------------------------------------------------------
static QString makeV2Payload(bool include_version = true,
                             const QString &version = "2.0",
                             bool null_sepsis_onset = false) {
  QString sepsis_onset_val = null_sepsis_onset ? "null" : "1712973600000";
  QString version_field =
      include_version ? QString(R"("version":"%1",)").arg(version) : "";
  return QString(R"({%1"patient_id":"P001","scenario":"sepsis",)"
                 R"("scenario_stage":"pre_sepsis","timestamp":1712973600000,)"
                 R"("hr":92.0,"bp_sys":135.0,"bp_dia":85.0,"o2_sat":98.0,)"
                 R"("temperature":37.2,"respiratory_rate":18.0,"wbc":11.5,)"
                 R"("lactate":1.2,"sirs_score":2,"qsofa_score":1,)"
                 R"("sepsis_stage":"sirs","sepsis_onset_ts":%2,)"
                 R"("quality":"good","source":"simulator"})")
      .arg(version_field, sepsis_onset_val);
}

static void initializeRuntimeSchemaOrFail() {
  try {
    MqttPayload::initializeValidator(kTestSchemaPath);
    ++g_pass;
  } catch (const std::exception &e) {
    std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__
              << "  Failed to initialize schema validator: " << e.what()
              << "\n";
    ++g_fail;
  }
}

// ---------------------------------------------------------------------------
// Test 1: parseVital with valid v2 JSON → VitalReading correct
// ---------------------------------------------------------------------------
static void test_parseVital_valid() {
  const QString json = makeV2Payload();

  VitalReading r = MqttPayload::parseVital(json);
  ASSERT_EQ(r.version, QString("2.0"));
  ASSERT_EQ(r.patient_id, QString("P001"));
  ASSERT_EQ(r.scenario, QString("sepsis"));
  ASSERT_EQ(r.scenario_stage, QString("pre_sepsis"));
  ASSERT_EQ(r.timestamp, static_cast<qint64>(1712973600000LL));
  ASSERT_TRUE(qAbs(r.hr - 92.0f) < 0.01f);
  ASSERT_TRUE(qAbs(r.bp_sys - 135.0f) < 0.01f);
  ASSERT_TRUE(qAbs(r.bp_dia - 85.0f) < 0.01f);
  ASSERT_TRUE(qAbs(r.o2_sat - 98.0f) < 0.01f);
  ASSERT_TRUE(qAbs(r.temperature - 37.2f) < 0.01f);
  ASSERT_TRUE(qAbs(r.respiratory_rate - 18.0f) < 0.01f);
  ASSERT_TRUE(qAbs(r.wbc - 11.5f) < 0.01f);
  ASSERT_TRUE(qAbs(r.lactate - 1.2f) < 0.01f);
  ASSERT_EQ(r.sirs_score, 2);
  ASSERT_EQ(r.qsofa_score, 1);
  ASSERT_EQ(r.sepsis_stage, QString("sirs"));
  ASSERT_TRUE(r.sepsis_onset_ts.has_value());
  ASSERT_EQ(*r.sepsis_onset_ts, static_cast<qint64>(1712973600000LL));
  ASSERT_EQ(r.quality, QString("good"));
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
// Test 9: Parse valid v2 JSON vital and verify UIModel is updated
// ---------------------------------------------------------------------------
static void test_uiModel_setVital() {
  UIModel model;

  const QString json =
      R"({"version":"2.0","patient_id":"P002","scenario":"healthy",)"
      R"("scenario_stage":"healthy","timestamp":1712973600000,)"
      R"("hr":65.0,"bp_sys":120.0,"bp_dia":80.0,"o2_sat":99.0,)"
      R"("temperature":36.6,"respiratory_rate":14.0,"wbc":7.5,)"
      R"("lactate":0.8,"sirs_score":0,"qsofa_score":0,)"
      R"("sepsis_stage":"none","sepsis_onset_ts":null,)"
      R"("quality":"good","source":"test"})";

  VitalReading r = MqttPayload::parseVital(json);
  model.setVital(r);

  // Verify formatted strings match expectations
  ASSERT_EQ(model.getHrValue(), QString("65 bpm"));
  ASSERT_EQ(model.getBpValue(), QString("120/80 mmHg"));
  ASSERT_EQ(model.getO2Value(), QString("99 %"));
  ASSERT_EQ(model.getQualityValue(), QString("good"));
  ASSERT_EQ(model.getSepsisStageValue(), QString("none"));
  ASSERT_EQ(model.getRespRateValue(), QString("14 breaths/min"));
  ASSERT_EQ(model.getSirsScoreValue(), QString("0"));
  ASSERT_EQ(model.getQsofaScoreValue(), QString("0"));
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
  // Missing "hr" field from an otherwise valid v2 payload
  const QString json =
      R"({"version":"2.0","patient_id":"P001","scenario":"sepsis",)"
      R"("scenario_stage":"pre_sepsis","timestamp":1712973600000,)"
      R"("bp_sys":135.0,"bp_dia":85.0,"o2_sat":98.0,"temperature":37.2,)"
      R"("respiratory_rate":18.0,"wbc":11.5,"lactate":1.2,"sirs_score":2,)"
      R"("qsofa_score":1,"sepsis_stage":"sirs","sepsis_onset_ts":null,)"
      R"("quality":"good","source":"simulator"})";
  ASSERT_THROWS(MqttPayload::parseVital(json), std::out_of_range);
}

// ---------------------------------------------------------------------------
// Test 11: parseVital rejects payload with wrong version → runtime_error
// ---------------------------------------------------------------------------
static void test_parseVital_rejects_wrong_version() {
  // version = "1.0" — must be rejected
  ASSERT_THROWS(MqttPayload::parseVital(makeV2Payload(true, "1.0")),
                std::runtime_error);
  // version = "3.0" — must be rejected
  ASSERT_THROWS(MqttPayload::parseVital(makeV2Payload(true, "3.0")),
                std::runtime_error);
}

// ---------------------------------------------------------------------------
// Test 12: parseVital rejects payload with missing version → runtime_error
// ---------------------------------------------------------------------------
static void test_parseVital_rejects_missing_version() {
  ASSERT_THROWS(MqttPayload::parseVital(makeV2Payload(false)),
                std::runtime_error);
}

// ---------------------------------------------------------------------------
// Test 13: parseVital handles sepsis_onset_ts: null → nullopt
// ---------------------------------------------------------------------------
static void test_parseVital_null_sepsis_onset() {
  const QString json = makeV2Payload(true, "2.0", true /*null onset*/);
  VitalReading r = MqttPayload::parseVital(json);
  ASSERT_TRUE(!r.sepsis_onset_ts.has_value());
}

// ---------------------------------------------------------------------------
// Test 14: validator initialization fails when schema file is missing
// ---------------------------------------------------------------------------
static void test_initializeValidator_missing_schema() {
  ASSERT_THROWS(
      MqttPayload::initializeValidator("/tmp/medtech/does-not-exist.json"),
      std::runtime_error);
}

// ---------------------------------------------------------------------------
// Test 15: parseVital rejects schema violations (type mismatch)
// ---------------------------------------------------------------------------
static void test_parseVital_contract_violation() {
  const QString invalid_type_payload =
      R"({"version":"2.0","patient_id":"P001","scenario":"sepsis",)"
      R"("scenario_stage":"pre_sepsis","timestamp":1712973600000,)"
      R"("hr":"bad","bp_sys":135.0,"bp_dia":85.0,"o2_sat":98.0,)"
      R"("temperature":37.2,"respiratory_rate":18.0,"wbc":11.5,)"
      R"("lactate":1.2,"sirs_score":2,"qsofa_score":1,)"
      R"("sepsis_stage":"sirs","sepsis_onset_ts":null,)"
      R"("quality":"good","source":"simulator"})";
  ASSERT_THROWS(MqttPayload::parseVital(invalid_type_payload),
                std::runtime_error);
}

// ---------------------------------------------------------------------------
// Test 16: Schema fixture validation — checks that the example v2 payload
//          satisfies the required-fields contract defined in
//          contracts/vitals/v2.0.json.  This is a lightweight structural
//          check (field presence + type category) rather than a full
//          JSON-Schema validator.
// ---------------------------------------------------------------------------
static void test_schema_fixture_validation() {
  // Load the vendored schema and parse its "required" array
  QFile schema_file(QString(MEDTECH_SOURCE_DIR) +
                    "/contracts/vitals/v2.0.json");
  if (!schema_file.open(QIODevice::ReadOnly)) {
    std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__
              << "  Cannot open contracts/vitals/v2.0.json\n";
    ++g_fail;
    return;
  }
  const QJsonDocument schema_doc =
      QJsonDocument::fromJson(schema_file.readAll());
  schema_file.close();

  if (schema_doc.isNull() || !schema_doc.isObject()) {
    std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__
              << "  contracts/vitals/v2.0.json is not valid JSON\n";
    ++g_fail;
    return;
  }
  ++g_pass; // schema file loads and parses

  const QJsonArray required = schema_doc.object().value("required").toArray();

  // Build an example fixture payload that should pass all required-field
  // checks (same payload as test_parseVital_valid)
  const QString fixture_str = makeV2Payload();
  const QJsonObject fixture =
      QJsonDocument::fromJson(fixture_str.toUtf8()).object();

  // Verify every field listed in schema "required" is present in the fixture
  bool all_present = true;
  for (const QJsonValue &v : required) {
    const QString field = v.toString();
    if (!fixture.contains(field)) {
      std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__
                << "  Fixture missing required field: " << field.toStdString()
                << "\n";
      ++g_fail;
      all_present = false;
    }
  }
  if (all_present) {
    ++g_pass;
  }

  // Verify the fixture is accepted by parseVital without exceptions
  bool parsed_ok = false;
  try {
    (void)MqttPayload::parseVital(fixture_str);
    parsed_ok = true;
  } catch (const std::exception &e) {
    std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__
              << "  Fixture rejected by parseVital: " << e.what() << "\n";
    ++g_fail;
  }
  if (parsed_ok) {
    ++g_pass;
  }
}

// ---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  // QCoreApplication required for QObject, signals, and Qt containers
  QCoreApplication app(argc, argv);

  std::cout << "Running MedTech Clinician UI unit tests (contract v2.0)...\n";
  initializeRuntimeSchemaOrFail();

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
  test_parseVital_rejects_wrong_version();
  test_parseVital_rejects_missing_version();
  test_parseVital_null_sepsis_onset();
  test_initializeValidator_missing_schema();
  test_parseVital_contract_violation();
  test_schema_fixture_validation();

  std::cout << g_pass << " tests passed, " << g_fail << " tests failed.\n";

  if (g_fail > 0) {
    return EXIT_FAILURE;
  }
  std::cout << "All tests passed \xE2\x9C\x85\n";
  return EXIT_SUCCESS;
}
