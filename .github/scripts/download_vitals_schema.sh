#!/usr/bin/env bash
set -euo pipefail

TAG="${1:?Usage: download_vitals_schema.sh <tag>}"
mkdir -p contracts/schemas/vitals
curl -fsSL \
  "https://raw.githubusercontent.com/chaithubk/medtech-telemetry-contract/${TAG}/schemas/vitals/vitals.schema.json" \
  -o contracts/schemas/vitals/vitals.schema.json

echo "Downloaded schema for ${TAG}"
