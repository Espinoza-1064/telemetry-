**Telemetry: ESP32 ALTITUDE AND VERTICAL SPEED STREAMER**
a small telemetry for a drone, built on an ESP32. It reads an MPU6050 IMU and a GPS module, estimates altitude and vertical speed and streams them as JSON over WiFi to a PC at about 10 Hz.

STATUS:
Work in progress. Streaming over Wifi works on the bench. The vertical velocity estimate still drifts, because it is the accelerometer integrated over time and the GPS only corrects altitude.

GOAL:
The aim is a first end to end. telemetry link form the sensor to a PC: sensor fusion on the ESP32, its own Wifi access point and a simple UDP protocol with a Python receiver.

HARDWARE:
- ESP32 dev board
- MPU6050 IMU over I2C
- GPS module over UART
Altitudes comes from the GPS alone.

HOW IT WORKS:
- The ESP32 runs its own access point at 192.168.4.1.
- Every cycle it blends the accelerometer motion with the latest GPS altitude.
- It listens on UDP port 4210. Once the PC sends a packet, the ESP32 remembers the sender and streams JSON to it at about 10HZ.

RUNNING IT:
1). Install PlatformIO and use this configuration
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
  lib_deps =
    bblanchon/ArduinoJson
    rfetick/MPU6050_light
    mikalhart/TinyGPSPlus

2). Upload the firmware and open the serial monitor. Keep the board still until the MPU6050 finishes calibrating on boot.

3).Join the drone_esp32 WiFi network from your computer.

4).In a terminal tab, run the receiver (standard library only)
   python3 import.py

