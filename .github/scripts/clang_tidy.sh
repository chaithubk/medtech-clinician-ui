#!/usr/bin/env bash
set -euo pipefail
find src -name "*.cpp" | xargs clang-tidy -p build 2>&1 || true
