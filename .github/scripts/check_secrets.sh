#!/usr/bin/env bash
set -euo pipefail
! grep -rn --include='*.cpp' --include='*.h' -iE '(password|secret|api_key)\s*=\s*"[^"]+"' src/ || (echo "::error::Hardcoded secret detected in source" && exit 1)
