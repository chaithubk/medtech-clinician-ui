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

## Parser contract

The clinician UI strictly enforces contract version `2.0`:

- If the MQTT payload is missing `version` or `version != "2.0"`, the message
  is **dropped** (parser throws, caller logs a warning and keeps the previous
  UI state).
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
