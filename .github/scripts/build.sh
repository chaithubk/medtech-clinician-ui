#!/usr/bin/env bash
set -euo pipefail
make -C build -j$(nproc)
