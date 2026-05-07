#pragma once

#include "../models/vital_reading.h"
#include <QString>

/**
 * @brief JSON payload parser for MQTT vital-sign messages.
 *
 * All parsing logic lives in this namespace; the class owns no state.
 */
namespace MqttPayload {

/**
 * @brief Load and cache the vitals JSON schema used for runtime validation.
 *
 * @param schema_path Absolute path to the schema JSON file.
 * @throws std::runtime_error if the schema file cannot be read/parsed.
 */
void initializeValidator(const QString &schema_path);

/**
 * @brief Parse a JSON vital-signs payload into a VitalReading.
 *
 * @param json_str UTF-8 JSON string received from the MQTT broker.
 * @return Populated VitalReading.
 * @throws std::runtime_error  if the JSON is syntactically invalid, schema is
 *                              not loaded, or payload violates the schema.
 * @throws std::out_of_range   if a required field is missing.
 */
VitalReading parseVital(const QString &json_str);

} // namespace MqttPayload
