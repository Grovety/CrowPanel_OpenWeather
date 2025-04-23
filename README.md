# ESP32-S3 Weather Panel for Elecrow Panel Avance 5.0

## Project Overview

This project is a weather dashboard for the Elecrow Panel Avance 5.0 based on ESP32-S3 and built with LVGL.

It features a simple touch-based interface with two main screens:

### 1. Settings Screen
- Configure Wi-Fi (Access Point + client mode)
- Adjust screen brightness
- Select city manually

### 2. Weather Display Screen
- Current weather:
  - Temperature
  - Feels like
  - Cloud coverage
  - Humidity
  - Wind speed
  - Atmospheric pressure
  - Sunrise and sunset time
- Today’s forecast: 4 timepoints (every 3 hours)
- 4-day forecast: day and night temperatures

### Data Sources
- Weather data: [openweather.com](https://openweather.com)
- City auto-detection (optional): [ip-api.com](http://ip-api.com)
- Time synchronization: [pool.ntp.org](https://www.pool.ntp.org)

---

## How to Build and Flash (via VSCode ESP-IDF Extension)

### Prerequisites

- [Visual Studio Code](https://code.visualstudio.com/)
- [ESP-IDF Extension for VSCode](https://github.com/espressif/vscode-esp-idf-extension)
- `esp-idf v5.4`

### 1. Build the Firmware
In Visual Studio Code:

- Open the command palette (`F1` or `Ctrl+Shift+P`)

- Run `ESP-IDF: Build your project`

- Wait for the build to complete successfully

### 3. Flash the Device
 - Run `ESP-IDF: Select Flash Method`
 - Select `UART`
 - Run `ESP-IDF: Select Port to use`
 - Choose the serial port your device is connected to
    (e.g. `COM3` on Windows or `/dev/ttyUSB0` on Linux)
 - Run `ESP-IDF: Flash your project`
 - Wait for flashing to complete

 - (Optional) Run ESP-IDF: `Monitor your device` to view logs and debug output