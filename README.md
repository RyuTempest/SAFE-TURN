# 🚦 SAFE TURN

**A Smart Blind Curve Warning System using ESP32 & LiDAR**

[![ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![TF-Luna](https://img.shields.io/badge/Sensor-TF--Luna%20LiDAR-green.svg)](https://www.benewake.com/en/tfmini.html)
[![ESP-NOW](https://img.shields.io/badge/Communication-ESP--NOW-orange.svg)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)

---

## 📌 Overview

**SAFE TURN** is an intelligent traffic safety system designed to prevent accidents on dangerous blind curves and narrow roads. The system uses two strategically placed ESP32 devices equipped with TF-Luna LiDAR sensors to detect approaching vehicles and provide early warnings to drivers on the opposite side of the curve.

### How It Works
1. **Vehicle Detection**: LiDAR sensors detect approaching vehicles within the configurable range
2. **Speed Calculation**: The system calculates vehicle speed in real-time using distance measurements
3. **Wireless Communication**: ESP-NOW protocol transmits warning signals instantly between devices
4. **Visual Feedback**: OLED displays show system status and detected vehicle speeds
5. **Warning Activation**: Relay-controlled warning lights/sirens alert drivers of incoming traffic

---

## ⚡ Key Features

### 🚗 **Advanced Vehicle Detection**
- **TF-Luna LiDAR sensors** with I2C communication for precise distance measurement
- Real-time speed calculation with 0.01-second precision
- Configurable detection threshold and range (default: 1.0m)
- Minimum speed threshold to filter false positives (1.0 km/h)

### 📡 **Wireless Communication**
- **ESP-NOW protocol** for ultra-low latency communication
- No WiFi network required - direct device-to-device communication
- Automatic retry mechanism with configurable attempts
- Bi-directional data transmission between both devices

### 🖥️ **Visual Interface**
- **128x64 OLED displays** on both devices
- Boot animation sequence with device identification
- Real-time speed display with smooth animation
- System status monitoring and feedback

### 🔔 **Warning System**
- **Relay control** for external warning devices (lights, sirens, signs)
- Configurable relay activation duration (default: 3 seconds)
- Automatic relay reset after warning period
- Multiple device support for comprehensive coverage

### ⚙️ **Configurable Parameters**
- Detection distance threshold
- Relay activation duration  
- Speed calculation sensitivity
- Display animation timing
- Communication retry settings

---

## 🔧 Hardware Requirements

### Core Components (Per Device)
| Component | Quantity | Notes |
|-----------|----------|-------|
| ESP32 Development Board | 1 | Any ESP32 variant with I2C support |
| TF-Luna LiDAR Sensor | 1 | Benewake TF-Luna with I2C interface |
| 128x64 OLED Display | 1 | ST7567 controller, I2C interface |
| Relay Module | 1 | 5V trigger, suitable for warning devices |
| Warning Device | 1 | LED lights, sirens, or electronic signs |

### Additional Requirements
- Power supply (5V/12V depending on warning devices)
- Weatherproof enclosures for outdoor installation
- Mounting hardware and cables
- Optional: Solar panels for remote installations

---

## 🔌 Pin Configuration

| Component | ESP32 Pin | Function | Notes |
|-----------|-----------|----------|-------|
| **LiDAR SDA** | 21 | I2C Data | Shared with OLED |
| **LiDAR SCL** | 22 | I2C Clock | Shared with OLED |
| **OLED SDA** | 21 | I2C Data | Shared with LiDAR |
| **OLED SCL** | 22 | I2C Clock | Shared with LiDAR |
| **Relay Control** | 5 | Digital Output | Controls warning devices |
| **Power** | 3.3V/5V | Power Supply | Depending on components |
| **Ground** | GND | Common Ground | All components |

### I2C Addresses
- **TF-Luna LiDAR**: `0x10`
- **OLED Display**: `0x3F`

---

## 📂 Project Structure

```
SAFE-TURN/
├── README.md                 # This file
├── device1/
│   └── device1.ino          # Device 1 code (Detection & Transmission)
├── device2/
│   └── device2.ino          # Device 2 code (Reception & Warning)
└── docs/
    ├── schematic-diagram.png  # Hardware schematic diagrams
    └── wiring-diagram.png   # Hardware connection diagrams

```

---

## ⚙️ Configuration Options

### Distance and Detection Settings
```cpp
#define DISTANCE_THRESHOLD 1.0      // Trigger distance (meters)
#define MAX_DETECT_RANGE 1.0        // Maximum detection range (meters)
```

### Timing Configuration
```cpp
#define RELAY_ON_DURATION 3000      // Warning duration (milliseconds)
#define RETRY_DELAY 100             // Communication retry delay (ms)
```

### Communication Settings
```cpp
#define CHANNEL 1                   // ESP-NOW channel
#define MAX_RETRIES 1               // Maximum transmission attempts
```

### Device MAC Addresses
Update these addresses in the code to match your ESP32 devices:
```cpp
// Device 1 broadcasts to Device 2
uint8_t broadcastAddress[] = {0xF4, 0x65, 0x0B, 0x48, 0x85, 0xB0};

// Device 2 broadcasts to Device 1  
uint8_t broadcastAddress[] = {0xF4, 0x65, 0x0B, 0x46, 0xF7, 0xE4};
```

---

## 🚀 Getting Started

### Prerequisites
1. **Arduino IDE** with ESP32 board package installed
2. Required libraries:
   - `Wire.h` (included with Arduino IDE)
   - `WiFi.h` (included with ESP32 package)
   - `esp_now.h` (included with ESP32 package)
   - `U8g2lib.h` - [Install from Library Manager](https://github.com/olikraus/u8g2)
   - `TFLI2C.h` - [Download from Benewake](https://github.com/budryerson/TFLuna-I2C)

### Installation Steps

1. **Clone or download** this repository
2. **Install required libraries** in Arduino IDE
3. **Update MAC addresses** in both device files to match your ESP32 boards
4. **Upload device1.ino** to the first ESP32
5. **Upload device2.ino** to the second ESP32
6. **Connect hardware** according to the pin configuration
7. **Test the system** in a controlled environment before deployment

### Finding ESP32 MAC Addresses
Use this code to find your ESP32 MAC addresses:
```cpp
#include "WiFi.h"
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}
void loop() {}
```

---

## 📊 System Operation

### Boot Sequence
1. **Hardware initialization** (I2C, pins, display)
2. **ESP-NOW setup** and peer registration
3. **Boot animation** with device identification
4. **System ready** - waiting for vehicle detection

### Detection Process
1. **Continuous LiDAR monitoring** (100ms intervals)
2. **Distance measurement** and speed calculation
3. **Threshold evaluation** (distance < 1.0m, speed > 1.0 km/h)
4. **Data transmission** to paired device
5. **Warning activation** on receiving device

### Display Information
- **"00.00"**: System ready, no detection
- **Speed values**: Detected vehicle speed (km/h)
- **Boot messages**: "SAFE TURN", "DEVICE X", "READY!"

---

## 🛠️ Troubleshooting

### Common Issues

**No communication between devices:**
- Verify MAC addresses are correctly configured
- Check ESP-NOW initialization in serial monitor
- Ensure both devices are powered and within range

**Inaccurate speed readings:**
- Calibrate LiDAR sensors for your installation height
- Adjust `DISTANCE_THRESHOLD` for your curve geometry
- Check I2C connections and addresses

**Relay not activating:**
- Verify relay module power requirements
- Check relay pin connections (default: pin 5)
- Test relay manually with digitalWrite commands

**Display issues:**
- Confirm OLED I2C address (usually 0x3C or 0x3F)
- Check SDA/SCL connections
- Verify display power supply voltage

---

## 🔒 Safety Considerations

- **Test thoroughly** in controlled environments before deployment
- **Use weatherproof enclosures** for outdoor installations
- **Ensure proper grounding** of all electrical components
- **Follow local regulations** for traffic safety devices
- **Regular maintenance** and calibration recommended
- **Backup power systems** for critical installations

---

## 🤝 Contributing

We welcome contributions to improve the SAFE TURN system! Please consider:

- **Bug reports** and feature requests via Issues
- **Code improvements** and optimizations
- **Documentation** updates and translations
- **Hardware compatibility** testing and reports

### Development Guidelines
- Follow Arduino coding standards
- Comment code thoroughly
- Test changes on actual hardware
- Update documentation as needed

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

---

## 📧 Support

For technical support, feature requests, or general questions:
- **Create an issue** on this repository
- **Check existing documentation** and troubleshooting guides
- **Test with minimal hardware setup** to isolate problems

---

## 🙏 Acknowledgments

- **Benewake** for TF-Luna LiDAR sensors and libraries
- **Espressif Systems** for ESP32 platform and ESP-NOW protocol
- **U8g2 Library** developers for excellent OLED display support
- **Arduino Community** for extensive documentation and examples

---

*Built with ❤️ for safer roads and communities*