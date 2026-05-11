#!/usr/bin/env bash
set -euo pipefail

INPUT_TAG="${1:-}"

TAG="${INPUT_TAG}"
if [[ -z "${TAG}" ]]; then
  TAG=$(gh api repos/chaithubk/medtech-telemetry-contract/releases/latest --jq '.tag_name' 2>/dev/null || true)
  if [[ -z "${TAG}" || "${TAG}" == "null" ]]; then
    TAG=$(gh api repos/chaithubk/medtech-telemetry-contract/tags --jq '.[0].name' 2>/dev/null || true)
  fi
fi

if [[ -z "${TAG}" || "${TAG}" == "null" ]]; then
  echo "::error::Could not resolve target contract tag."
  exit 1
fi

COMMIT=$(gh api "repos/chaithubk/medtech-telemetry-contract/commits/${TAG}" --jq '.sha')
if [[ -z "${COMMIT}" || "${COMMIT}" == "null" ]]; then
  echo "::error::Could not resolve commit for tag ${TAG}."
  exit 1
fi

if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
  {
    echo "tag=${TAG}"
    echo "commit=${COMMIT}"
  } >> "$GITHUB_OUTPUT"
fi

echo "Resolved contract tag: ${TAG}"
echo "Resolved contract commit: ${COMMIT}"
