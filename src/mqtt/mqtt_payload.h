#pragma once

#include "../models/vital_reading.h"
#include <QString>

namespace MqttPayload {
VitalReading parseVital(const QString &json_str);
}
