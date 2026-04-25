FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake g++ gdb pkg-config git vim nano curl wget tree htop \
    clang-format clang-tidy \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libx11-dev \
    libxkbcommon-dev \
    qt6-base-dev qt6-declarative-dev libqt6opengl6-dev libqt6svg6-dev \
    qml6-module-qtquick \
    qml6-module-qtquick-controls \
    qml6-module-qtquick-layouts \
    qml6-module-qtquick-templates \
    qml6-module-qtquick-window \
    qml6-module-qtqml-workerscript \
    libmosquitto-dev \
    mosquitto \
    mosquitto-clients \
    xvfb \
    scrot \
    --no-install-recommends \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

WORKDIR /workspace

ENV CMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake:/usr/lib/x86_64-linux-gnu:/usr/lib/cmake
ENV PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig:/usr/share/pkgconfig:/usr/lib/pkgconfig
ENV TERM=xterm-256color
ENV LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:/usr/lib:$LD_LIBRARY_PATH

# Set paths for all shells
RUN echo "export CMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake:/usr/lib/x86_64-linux-gnu:/usr/lib/cmake" >> /etc/profile && \
    echo "export PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig:/usr/share/pkgconfig:/usr/lib/pkgconfig" >> /etc/profile && \
    echo "export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:/usr/lib:\$LD_LIBRARY_PATH" >> /etc/profile

CMD ["/bin/bash"]