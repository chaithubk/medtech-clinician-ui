#!/usr/bin/env bash
set -euo pipefail
cd build
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
lcov --list coverage.info
