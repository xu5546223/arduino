# Pet Feeding System

[中文](#中文) | [English](#english)

## 中文

### 專案簡介
這是一個基於 Arduino 的寵物餵食系統專案，包括三個主要部分：鏡頭串流、Wi-Fi 連接資訊以及機器控制程式碼。專案使用 WebSocket 進行鏡頭的即時通信，並使用 MQTT 通過 Wi-Fi 控制餵食機構。機器使用 Arduino Uno R4 WiFi，而鏡頭模組使用 ESP32-CAM。

### 功能列表
- 通過 WebSocket 進行即時鏡頭串流
- Wi-Fi 連接
- 通過 MQTT 控制寵物餵食機器

### 文件說明

#### 1. `websocket_camera_stream.ino`
這個檔案包含了處理鏡頭串流的程式碼，使用 WebSocket 進行即時影像傳輸，適用於 ESP32-CAM。

#### 2. `arduino_secrets.h`
這個檔案儲存了 Wi-Fi 的 SSID 和密碼，用於連接網絡。

#### 3. `machine2.0.ino`
這個檔案包含了機器控制的主要程式碼，適用於 Arduino Uno R4 WiFi。

### 安裝步驟

1. **準備硬體**
   - Arduino Uno R4 WiFi 板
   - ESP32-CAM 模組
   - 其他必要的機械和電子元件（如伺服電機、餵食器結構）

2. **設定 Arduino IDE**
   - 下載並安裝 [Arduino IDE](https://www.arduino.cc/en/software)
   - 安裝相應的板卡和庫（例如 ESP32 的板卡管理工具和 WebSocket 庫）

3. **上傳程式碼**
   - 將 `arduino_secrets.h` 檔案中的 Wi-Fi SSID 和密碼替換為你自己的資訊
   - 將 `websocket_camera_stream.ino` 上傳到 ESP32-CAM
   - 將 `machine2.0.ino` 上傳到 Arduino Uno R4 WiFi

### 使用說明

1. **啟動系統**
   - 通過 Arduino IDE 將程式碼上傳到相應的硬體
   - 確保相機模組和 Wi-Fi 模組正確連接並工作

2. **連接 Wi-Fi**
   - 系統將自動連接到 `arduino_secrets.h` 中配置的 Wi-Fi 網絡

3. **開始控制**
   - 使用 WebSocket 進行鏡頭的即時串流
   - 使用 MQTT 控制寵物餵食機器
   - 通過瀏覽器或其他 WebSocket/MQTT 客戶端查看即時影像並控制寵物餵食機器

## English

### Project Overview
This project is a pet feeding system based on Arduino, comprising three main parts: camera streaming, Wi-Fi connection information, and machine control code. The project uses WebSocket for real-time communication with the camera and MQTT for controlling the feeding mechanism over Wi-Fi. The machine uses an Arduino Uno R4 WiFi, and the camera module uses an ESP32-CAM.

### Features
- Real-time camera streaming via WebSocket
- Wi-Fi connection
- Pet feeding machine control via MQTT

### Files

#### 1. `websocket_camera_stream.ino`
This file contains the code for handling camera streaming using WebSocket for real-time image transmission, suitable for ESP32-CAM.

#### 2. `arduino_secrets.h`
This file stores the Wi-Fi SSID and password for network connection.

#### 3. `machine2.0.ino`
This file contains the main control code for the pet feeding machine, suitable for Arduino Uno R4 WiFi.

### Setup Instructions

1. **Prepare Hardware**
   - Arduino Uno R4 WiFi
   - ESP32-CAM module
   - Other necessary mechanical and electronic components (e.g., servo motor, feeding structure)
   
2. **Configure Arduino IDE**
   - Download and install [Arduino IDE](https://www.arduino.cc/en/software)
   - Install the appropriate board and libraries (e.g., ESP32 board manager and WebSocket library)

3. **Upload Code**
   - Replace the Wi-Fi SSID and password in the `arduino_secrets.h` file with your own information
   - Upload `websocket_camera_stream.ino` to the ESP32-CAM
   - Upload `machine2.0.ino` to the Arduino Uno R4 WiFi

### Usage Instructions

1. **Start the System**
   - Upload the code to the respective hardware using Arduino IDE
   - Ensure the camera module and Wi-Fi module are correctly connected and functioning

2. **Connect to Wi-Fi**
   - The system will automatically connect to the Wi-Fi network configured in `arduino_secrets.h`

3. **Control the Machine**
   - Use WebSocket for real-time camera streaming
   - Use MQTT for controlling the pet feeding mechanism
   - View real-time images and control the pet feeding machine through a web browser or other WebSocket/MQTT client
