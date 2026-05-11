#!/usr/bin/env bash
set -euo pipefail
find src tests -name "*.cpp" -o -name "*.h" | xargs clang-format --dry-run --Werror
