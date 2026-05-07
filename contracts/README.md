# MedTech Vitals Telemetry Contract (Vendored)

## Source of Truth

The canonical contract is maintained in:

> **Repository:** [chaithubk/medtech-telemetry-contract](https://github.com/chaithubk/medtech-telemetry-contract)  
> **Pinned tag:** `v2.0.0`  
> **Schema path:** `schemas/vitals/v2.0.json`

The file `contracts/vitals/v2.0.json` in this repository is a verbatim copy
vendored at the tag above. Do **not** edit it locally — instead follow the
update procedure below.

## What is vendored here

| File | Origin |
|------|--------|
| `contracts/vitals/v2.0.json` | `medtech-telemetry-contract@v2.0.0 / schemas/vitals/v2.0.json` |
| `contracts/VITALS_CONTRACT_VERSION.txt` | Pinned tag string (`v2.0.0`) |

## Runtime validation contract

The clinician UI resolves the runtime schema path from:

- `MEDTECH_VITALS_SCHEMA` (if set)
- default: `/usr/share/medtech/contracts/vitals/current.json`

At startup the UI loads that file and then validates every payload against it.

- If the schema file is missing/unreadable, vitals stream enters a persistent
  global telemetry error state (fail closed).
- If any MQTT payload violates the schema contract (required/type/enum/const),
  vitals stream enters the same persistent global telemetry error state.
- `sepsis_onset_ts` may be `null` (sepsis not yet detected) — represented as
  `std::nullopt` in `VitalReading::sepsis_onset_ts`.

## Update procedure

1. In `chaithubk/medtech-telemetry-contract`, create a new release tag (e.g.
   `v2.1.0`).
2. Copy the updated schema:
   ```sh
   # from repo root
   curl -fsSL \
     "https://raw.githubusercontent.com/chaithubk/medtech-telemetry-contract/<NEW_TAG>/schemas/vitals/v2.0.json" \
     -o contracts/vitals/v2.0.json
   echo "<NEW_TAG>" > contracts/VITALS_CONTRACT_VERSION.txt
   ```
3. Update the parsing code in `src/mqtt/mqtt_payload.cpp` and the
   `VitalReading` struct in `src/models/vital_reading.h` to reflect any
   schema changes.
4. Update tests in `tests/test_mqtt_client.cpp`.
5. Open a PR — CI must pass before merging.
