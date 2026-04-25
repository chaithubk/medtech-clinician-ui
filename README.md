# MedTech Clinician UI

A Qt6/QML real-time clinician dashboard that displays patient vital signs received over MQTT.

---

## Prerequisites

- [Docker](https://www.docker.com/) or [VS Code Dev Containers](https://code.visualstudio.com/docs/devcontainers/containers) extension

---

## Getting Started (Dev Container — Recommended)

1. Open this repository in VS Code.
2. When prompted, click **Reopen in Container** (or run `Dev Containers: Reopen in Container` from the command palette).
3. The container will build and install all dependencies automatically (Qt6, QML modules, mosquitto, Xvfb, etc.).

### Build

```sh
mkdir -p build && cd build && cmake .. && make -j$(nproc)
```

Or use the CMake Tools extension — it will configure automatically on container open.

---

## Running the Dashboard Locally (with UI)

The dashboard uses a virtual X display (Xvfb) to render the Qt/QML UI inside the headless container. This lets you take screenshots or forward the display via X11.

### 1. Start the virtual display and broker

```sh
# Start virtual X display (skip if already running)
pgrep Xvfb > /dev/null || Xvfb :99 -screen 0 1280x800x24 &

# Start MQTT broker (skip if already running)
pgrep mosquitto > /dev/null || mosquitto -d
```

### 2. Run the dashboard

```sh
DISPLAY=:99 QT_QPA_PLATFORM=xcb ./bin/dashboard &
```

You should see in the terminal:
```
[INFO] MQTT connected to broker
[INFO] MQTT connected — subscribing to vitals topic
```

### 3. Send test MQTT vital signs

```sh
mosquitto_pub -h localhost -t "medtech/vitals/latest" \
  -m "{\"timestamp\":$(date +%s%3N),\"hr\":72.5,\"bp_sys\":120.0,\"bp_dia\":80.0,\"o2_sat\":98.5,\"temperature\":36.8,\"quality\":95,\"source\":\"test-simulator\"}"
```

### 4. Take a screenshot to verify the UI

```sh
DISPLAY=:99 scrot /tmp/dashboard_screenshot.png
```

Open `dashboard_screenshot.png` in VS Code Explorer to view the rendered dashboard.

---

## MQTT Payload Format

The app subscribes to the topic `medtech/vitals/latest` (configurable via `MQTT_TOPIC_VITALS` env var).

| Field | Type | Description |
|-------|------|-------------|
| `timestamp` | integer | Unix epoch in **milliseconds** |
| `hr` | float | Heart rate (bpm) |
| `bp_sys` | float | Systolic blood pressure (mmHg) |
| `bp_dia` | float | Diastolic blood pressure (mmHg) |
| `o2_sat` | float | SpO₂ (%) |
| `temperature` | float | Body temperature (°C) |
| `quality` | int | Signal quality score (0–100) |
| `source` | string | Data source label (e.g. `"bedside-monitor"`) |

> **Note:** Use `$(date +%s%3N)` for the timestamp to avoid the **"Stale Data"** warning, which triggers when data is older than 5 seconds.

---

## Configuration

All settings can be overridden with environment variables:

| Variable | Default | Description |
|----------|---------|-------------|
| `MQTT_BROKER` | `localhost` | MQTT broker hostname |
| `MQTT_PORT` | `1883` | MQTT broker port |
| `MQTT_TOPIC_VITALS` | `medtech/vitals/latest` | Topic to subscribe to |
| `MQTT_QOS` | `1` | MQTT QoS level |
| `DATA_STALE_TIMEOUT_MS` | `5000` | Stale data threshold (ms) |
| `WINDOW_WIDTH` | `1920` | Dashboard window width |
| `WINDOW_HEIGHT` | `1080` | Dashboard window height |

Example — connect to an external broker:
```sh
MQTT_BROKER=192.168.1.100 DISPLAY=:99 QT_QPA_PLATFORM=xcb ./bin/dashboard &
```

---

## Running Tests

```sh
cd build && ctest --output-on-failure
```

Or run the test binary directly:
```sh
./bin/test_mqtt_client
```

---

## Project Structure

```
.devcontainer/   Dev container config (Dockerfile + devcontainer.json)
qml/             QML UI files (Dashboard, VitalCard, AlarmPanel)
src/
  main.cpp
  models/        VitalReading, UIModel
  mqtt/          MQTTClient, payload parser
  ui/            DashboardWindow, VitalRenderer
  utils/         Logger, Config
tests/           Unit tests
```
