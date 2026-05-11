#!/usr/bin/env bash
set -euo pipefail

PINNED="${1:-}"
TARGET="${2:-}"

if [[ "${PINNED}" == "${TARGET}" ]]; then
  DRIFT="false"
  echo "Contract is up to date (${PINNED})."
else
  DRIFT="true"
  echo "Contract drift detected: ${PINNED} -> ${TARGET}"
fi

if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
  echo "drift=${DRIFT}" >> "$GITHUB_OUTPUT"
fi
