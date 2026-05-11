# MedTech Vitals Telemetry Contract (Vendored)

## Governance model

This repository consumes the telemetry contract through a structured pin file:

- `contracts/contract-pin.json`

The pin file is the single source of truth for:

- contract source repository
- upstream schema path
- pinned contract tag
- pinned upstream commit SHA
- pinned schema revision digest
- compatibility classification (`patch`, `minor`, `breaking`, or `unknown`)
- vendored schema path used by tests/build

Do not edit vendored schema files manually without updating
`contracts/contract-pin.json` in the same PR.

## What is vendored here

| File | Purpose |
|------|---------|
| `contracts/contract-pin.json` | Structured contract pin metadata |
| `contracts/schemas/vitals/vitals.schema.json` | Vendored schema copy matching the pin |

## Runtime validation contract

The clinician UI resolves the runtime schema path in this order:

1. `MEDTECH_VITALS_SCHEMA` (if set)
2. `consumer.runtime_schema_path` from `MEDTECH_CONTRACT_PIN` (default: `/usr/share/medtech/contracts/contract-pin.json`)
3. fallback: `/usr/share/medtech/contracts/schemas/vitals/vitals.schema.json`

At startup the UI loads that file and validates every incoming payload.

- If the schema file is missing/unreadable, vitals stream enters a persistent
  global telemetry error state (fail closed).
- If payloads violate schema constraints, vitals stream enters the same
  persistent global telemetry error state.
- Unknown future fields are tolerated when the loaded schema allows them.

## Update procedure

### Option A — GitHub Actions (recommended)

1. Trigger **Vendor Telemetry Contract** workflow in this repository (Actions tab).
2. Optionally provide a target tag (defaults to latest release tag).
3. Review the automated PR and merge once CI is green.

### Option B — Manual update

1. Pick the new upstream contract tag/commit in `chaithubk/medtech-telemetry-contract`.
2. Vendor the canonical schema path:
   ```sh
   curl -fsSL \
     "https://raw.githubusercontent.com/chaithubk/medtech-telemetry-contract/<NEW_TAG>/schemas/vitals/vitals.schema.json" \
     -o contracts/schemas/vitals/vitals.schema.json
   ```
3. Compute schema digest and update `contracts/contract-pin.json`:
   - `pin.tag`
   - `pin.upstream_commit` (if known)
   - `pin.schema_revision` (sha256 digest)
   - `compatibility.change_type`
   - `consumer.expected_payload_version` if changed
4. Update parser/model/tests as needed for any schema changes.
5. Run `tools/check_ci.sh` and open a PR with contract compatibility rationale.
