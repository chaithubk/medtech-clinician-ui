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
 * @brief Parse a JSON vital-signs payload into a VitalReading.
 *
 * @param json_str UTF-8 JSON string received from the MQTT broker.
 * @return Populated VitalReading.
 * @throws std::runtime_error  if the JSON is syntactically invalid.
 * @throws std::out_of_range   if a required field is missing.
 */
VitalReading parseVital(const QString &json_str);

} // namespace MqttPayload
