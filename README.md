# 🌤️ ESP32-S3 Weather Panel for Elecrow Panel Avance 5.0

## Project Overview

This project is a weather dashboard for the Elecrow Panel Avance 5.0 based on ESP32-S3 and built with LVGL.

It features a simple touch-based interface with two main screens:

### 1. ⚙️ Settings Screen
- Configure Wi-Fi (Access Point + client mode) 📶
- Adjust screen brightness 💡
- Select city manually 🏙️

### 2. 🌦️ Weather Display Screen
- Current weather:
  - 🌡️ Temperature
  - 🤔 Feels like
  - ☁️ Cloud coverage
  - 💧 Humidity
  - 💨 Wind speed
  - 📉 Atmospheric pressure
  - 🌅 Sunrise and 🌇 sunset time
- Today’s forecast: 4 timepoints (every 3 hours) ⏰
- 4-day forecast: day and night temperatures 📆
- **Set city by voice:** Tap the 🎤 microphone icon and say, for example, “Show me the weather in Dubai” (see below).

### Data Sources
- Weather data: [openweather.com](https://openweather.com) 🌐
- City auto-detection (optional): [ip-api.com](http://ip-api.com) 📍
- Time synchronization: [pool.ntp.org](https://www.pool.ntp.org) 🕒
- Voice recognition: [wit.ai](https://wit.ai) 🗣️

---

## 🎤 Voice City Selection

You can set the city for the weather display using your voice.

- Tap the **microphone icon** on the Weather screen.  
- Say, for example:  
  `"Show me the weather in Dubai"`
- The system will recognize your speech and automatically update the weather location for 10 seconds. Then return to configured.

> **Note:**  
> For the voice city selection feature to work, the `api.wit.ai` service must be accessible from your Wi-Fi network.  
> If access to `api.wit.ai` is restricted in your region, you may need to use a VPN. 🔒

---

## 🔑 API Keys Required

The following API keys are required to use all features of the project:

- **OpenWeather**  
  Register at [openweather.com](https://openweather.com) and get your API key.  
  Enter your key in:  
  `sources/entities/Weather.h`

- **Voice Recognition (wit.ai)**  
  For the voice city selection feature, register at [wit.ai](https://wit.ai) and obtain an API key.  
  Enter your key in:  
  `sources/entities/VoiceCityRecognizer.h`

---

## 🚀 How to Build and Flash (via VSCode ESP-IDF Extension)

### Prerequisites

- [Visual Studio Code](https://code.visualstudio.com/) 🖥️
- [ESP-IDF Extension for VSCode](https://github.com/espressif/vscode-esp-idf-extension) 🧩
- `esp-idf v5.4` ⚙️

### 1. 🛠️ Build the Firmware
In Visual Studio Code:

- Open the command palette (`F1` or `Ctrl+Shift+P`)
- Run `ESP-IDF: Build your project`
- Wait for the build to complete successfully ✅

### 2. 🔌 Flash the Device
- Run `ESP-IDF: Select Flash Method`
- Select `UART`
- Run `ESP-IDF: Select Port to use`
- Choose the serial port your device is connected to (e.g. `COM3` on Windows or `/dev/ttyUSB0` on Linux)
- Run `ESP-IDF: Flash your project`
- Wait for flashing to complete 🚦

- *(Optional)* Run `ESP-IDF: Monitor your device` to view logs and debug output 📝

---

If you have any questions or need additional help, feel free to reach out! 😎
