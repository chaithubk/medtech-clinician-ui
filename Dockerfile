FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake g++ gdb pkg-config git \
    clang-format clang-tidy clang \
    libgl1-mesa-dev libglu1-mesa-dev libx11-dev libxkbcommon-dev \
    qt6-base-dev qt6-declarative-dev libqt6opengl6-dev libqt6svg6-dev \
    qml6-module-qtquick \
    qml6-module-qtquick-controls \
    qml6-module-qtquick-layouts \
    qml6-module-qtquick-templates \
    qml6-module-qtquick-window \
    qml6-module-qtqml-workerscript \
    libmosquitto-dev \
    --no-install-recommends \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

ENV CMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake:/usr/lib/x86_64-linux-gnu:/usr/lib/cmake
ENV PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig:/usr/share/pkgconfig:/usr/lib/pkgconfig
ENV QT_QPA_PLATFORM=offscreen
ENV LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:/usr/lib

COPY . .

RUN mkdir -p build && cd build && cmake .. && make -j$(nproc)

CMD ["/bin/bash"]
