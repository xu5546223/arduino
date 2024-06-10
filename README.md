# Pet Feeding System

## Project Overview
This project is a pet feeding system based on Arduino, comprising three main parts: camera streaming, Wi-Fi connection information, and machine control code. The project uses WebSocket for real-time communication and Wi-Fi for remote control. The machine uses an Arduino Uno R4 WiFi, and the camera module uses an ESP32-CAM.

## Features
- Real-time camera streaming
- Wi-Fi connection
- Pet feeding machine control

## Files

### 1. `websocket_camera_stream.ino`
This file contains the code for handling camera streaming using WebSocket for real-time image transmission, suitable for ESP32-CAM.

### 2. `arduino_secrets.h`
This file stores the Wi-Fi SSID and password for network connection.

### 3. `machine2.0.ino`
This file contains the main control code for the pet feeding machine, suitable for Arduino Uno R4 WiFi.

## Setup Instructions

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

## Usage Instructions

1. **Start the System**
   - Upload the code to the respective hardware using Arduino IDE
   - Ensure the camera module and Wi-Fi module are correctly connected and functioning

2. **Connect to Wi-Fi**
   - The system will automatically connect to the Wi-Fi network configured in `arduino_secrets.h`

3. **Control the Machine**
   - Use WebSocket for real-time communication and control
   - View real-time images and control the pet feeding machine through a web browser or other WebSocket client

## Contributing
Any form of contribution is welcome! If you have any suggestions for improvement or find any issues, please let us know by submitting an issue or a pull request.

## Contact
If you have any questions, please contact: `your-email@example.com`
