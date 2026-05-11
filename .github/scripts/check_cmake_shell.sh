#!/usr/bin/env bash
set -euo pipefail
if grep -iE 'execute_process|system\(' CMakeLists.txt src/CMakeLists.txt 2>/dev/null; then
  echo "::warning::Shell execution found in CMake — review manually"
fi
