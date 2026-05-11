#!/usr/bin/env bash
set -euo pipefail
if [[ "${{ needs.lint.result }}"       != "success" || \
      "${{ needs.test.result }}"       != "success" || \
      "${{ needs.security.result }}"   != "success" || \
      "${{ needs.smoke-test.result }}" != "success" ]]; then
  echo "::error::One or more CI jobs failed"
  exit 1
fi
echo "All CI checks passed"
