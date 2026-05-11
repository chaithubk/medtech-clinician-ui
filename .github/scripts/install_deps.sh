#!/usr/bin/env bash
set -euo pipefail
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake g++ pkg-config \
  clang-format clang-tidy \
  libgl1-mesa-dev libglu1-mesa-dev libx11-dev libxkbcommon-dev \
  qt6-base-dev qt6-declarative-dev libqt6opengl6-dev libqt6svg6-dev \
  qml6-module-qtquick \
  qml6-module-qtquick-controls \
  qml6-module-qtquick-layouts \
  qml6-module-qtquick-templates \
  qml6-module-qtquick-window \
  qml6-module-qtqml-workerscript \
  libmosquitto-dev
