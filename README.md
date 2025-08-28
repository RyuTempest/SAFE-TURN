# 🚦 SAFE TURN

**A Smart Blind Curve Warning System using ESP32 & LiDAR**

---

## 📌 Overview
**SAFE TURN** is a dual-ESP32 system designed to reduce accidents on blind curves.  
It uses **TF-Luna LiDAR sensors** to detect approaching vehicles and **ESP-NOW wireless communication** to trigger a relay on the other side of the curve.  

Each device includes an **OLED display** to show real-time system status.  

---

## 📂 Code Files
- `device1.ino` → Code for **Device 1 (Detection & Transmitter)**
- `device2.ino` → Code for **Device 2 (Receiver & Warning Relay)**

---

## ⚡ Features 
- 🚘 Vehicle detection using **TF-Luna LiDAR (I2C)**
- 📏 Configurable detection threshold (`DISTANCE_THRESHOLD`)
- 🔔 Relay control for activating **warning lights/sirens** (`RELAY_ON_DURATION`)
- 📡 **ESP-NOW wireless communication** between Device 1 & Device 2  
  (low-latency, no WiFi required)
- 🖥️ OLED display feedback for system monitoring
- ♻️ Automatic relay reset after a set time

---

## 🔌 Pin Configuration
| Component | ESP32 Pin | Notes             |
|-----------|-----------|------------------|
| LiDAR SDA | 21        | I2C data          |
| LiDAR SCL | 22        | I2C clock         |
| Relay     | 5         | Trigger pin       |
| OLED SDA  | 21        | Shared with LiDAR |
| OLED SCL  | 22        | Shared with LiDAR |
