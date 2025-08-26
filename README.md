# Smart Farm Monitoring & Control System

This project is a **Smart Farm** system that integrates various sensors and actuators to monitor and automate key agricultural processes. It utilizes **ESP32** microcontrollers to handle sensor data, control devices, and provide a **web-based UI** for real-time monitoring and manual control.

## Features

- **Environmental Monitoring**  
  - **Temperature & Humidity Sensor (AHT10)**  
  - **Gas Sensors (MQ135 & MQ7) for Air Quality & Biogas Detection**  
  - **Flame Sensor for Fire Detection**  
  - **Soil Moisture Sensor for Irrigation Control**  
  
- **Actuators & Controls**  
  - **Humidifier for Mushroom Chamber**  
  - **Exhaust Fan for Ventilation**  
  - **Motor for Automated Irrigation**  
  - **LED Display for Real-time Data Visualization**  
  - **Keypad for Manual Device Control**  
  - **Servo Motor for Mechanized Access Control**  
  
- **Power & Connectivity**  
  - **Solar Panel for Battery Charging**  
  - **Wi-Fi Enabled Web UI for Remote Monitoring & Control**  

## Hardware Components

- **ESP32 Microcontroller**
- **AHT10 Temperature & Humidity Sensor**
- **MQ135 & MQ7 Gas Sensors**
- **Flame Sensor**
- **Soil Moisture Sensor**
- **Servo Motor**
- **Humidifier**
- **Exhaust Fan**
- **LED Display (MAX7219-based)**
- **Keypad (4x4 Matrix)**
- **Water Pump/Motor**
- **Solar Panel & Battery System**

## Software Components

### 1. `esp1.ino` - Keypad & Servo Control  
This file handles:
- Keypad-based device control
- Servo motor for access mechanism
- Web server for battery voltage monitoring & pin setup

### 2. `esp2.ino` - Sensor Data & Display  
This file manages:
- Reading temperature, humidity, and gas levels
- Displaying real-time data on LED display
- Controlling the humidifier, fan, and irrigation system based on threshold values
- Web-based monitoring and control of thresholds

## Web UI Functionality

- **Real-time sensor data visualization**
- **Remote control of actuators**
- **Customizable threshold values for automation**
- **Manual override options via Keypad**

## Setup Instructions

1. **Install Required Libraries**  
   Add the following libraries to your **Arduino IDE**:
   - `WiFi.h`
   - `WebServer.h`
   - `Adafruit AHT10`
   - `Keypad.h`
   - `ESP32Servo.h`
   - `MD_Parola` & `MD_MAX72XX` for LED display

2. **Configure Wi-Fi Credentials**  
   In both `esp1.ino` and `esp2.ino`, modify:
   ```cpp
   #define SSID "your_wifi_ssid"
   #define PASSWORD "your_wifi_password"
   ```

3. **Upload the Code**  
   - Flash `esp1.ino` to one ESP32 board (handles keypad & servo).
   - Flash `esp2.ino` to another ESP32 board (handles sensors & actuators).

4. **Access Web UI**  
   - After setup, the ESP32 will provide an IP address in the Serial Monitor.
   - Open the IP address in a browser to access the control panel.

## Future Enhancements

- **MQTT Integration for IoT Cloud Control**
- **Mobile App Interface**
- **Data Logging & Analytics Dashboard**

---

This project is an **open-source** initiative aimed at automating farms using IoT. Contributions & improvements are welcome! 🚀
