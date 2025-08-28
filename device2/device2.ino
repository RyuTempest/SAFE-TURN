// ESP 32 DEVICE 2
#include <Wire.h>     // Handles I2C communication between ESP32 and peripherals
#include <U8g2lib.h>  // Library for controlling graphical LCDs
#include <TFLI2C.h>   // Library for using TF-Luna LiDAR over I2C
#include <esp_now.h>  // ESP-NOW library for wireless communication
#include <WiFi.h>     // WiFi library required for ESP-NOW

// ========== CONFIGURATION ==========
// -- LiDAR and Relay Settings --
#define LIDAR_I2C_ADDR 0x10        
#define RELAY_PIN 5
#define RELAY_ON_DURATION 3000     // <-- Adjust this to change how long the relay stays on (in ms)
#define DISTANCE_THRESHOLD 1.0     // <-- Set trigger distance for incoming vehicle
#define MAX_DETECT_RANGE 1.0       // <-- Max expected range in meters for LiDAR
#define SDA_PIN 21
#define SCL_PIN 22

// -- ESP-NOW Settings --
#define CHANNEL 1
#define MAX_RETRIES 1
#define RETRY_DELAY 100  // ms

// Structure to hold speed data for transmission
struct SpeedData {
    float speed;
    bool isValid;
};

// Broadcast address for Device 1 (ESP32 Board 1)
uint8_t broadcastAddress[] = {0xF4, 0x65, 0x0B, 0x46, 0xF7, 0xE4};

// ========== DISPLAY SETUP ==========
U8G2_ST7567_ENH_DG128064I_F_HW_I2C u8g2(U8G2_R0, SCL_PIN, SDA_PIN, U8X8_PIN_NONE);
TFLI2C tflI2C;

// ========== GLOBAL VARIABLES ==========
float currentDistance = 0.0;
float previousDistance = 0.0;
unsigned long previousTime = 0;
esp_now_peer_info_t peerInfo;
bool relayActive = false;
unsigned long relayStartTime = 0;

// ========== ESP-NOW CALLBACKS ==========
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        Serial.println("Speed data sent successfully");
    } else {
        Serial.println("Error sending speed data");
    }
}

void OnDataRecv(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len) {
    if (data_len == sizeof(SpeedData)) {
        SpeedData receivedData;
        memcpy(&receivedData, data, sizeof(SpeedData));
        
        if (receivedData.isValid) {
            Serial.print("Received speed: ");
            Serial.println(receivedData.speed);
            displaySpeed(receivedData.speed);
            activateRelayAndDisplaySpeed(receivedData.speed);
            
            // Final speed display delay
            delay(1000);  // Hold the final speed on screen for 1 second
        }
    }
}

// ========== SETUP ==========
void setup() {
    Serial.begin(115200);
    Serial.println("\n\n=== SAFE TURN SYSTEM - DEVICE 2 STARTING ===");
    
    pinMode(SDA_PIN, INPUT_PULLUP);
    pinMode(SCL_PIN, INPUT_PULLUP);
    Wire.begin(SDA_PIN, SCL_PIN);

    u8g2.setI2CAddress(0x3F << 1);
    u8g2.begin();
    u8g2.clearBuffer();

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);

    // Initialize ESP-NOW
    Serial.println("Initializing ESP-NOW...");
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    Serial.println("ESP-NOW initialized successfully");

    // Register callbacks
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    Serial.println("Callbacks registered");

    // Add peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = CHANNEL;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
    Serial.println("Broadcast peer added successfully");
    Serial.println("ESP-NOW connection established!");
    Serial.println("Waiting for vehicle detection...");

    previousTime = millis();
    bootAnimation();
}

// ========== BOOT ANIMATION ==========
void bootAnimation() {
    const char* message = "SAFE TURN";
    const char* deviceMessage = "DEVICE 2";
    const char* readyMessage = "READY!";
    int16_t textWidth;

    // Display main message
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.clearBuffer();
    textWidth = u8g2.getStrWidth(message);
    u8g2.setCursor((128 - textWidth) / 2, 32);
    u8g2.drawStr((128 - textWidth) / 2, 32, message);
    u8g2.sendBuffer();
    delay(1000);

    // Blinking animation
    for (int i = 0; i < 3; i++) {
        u8g2.clearBuffer();
        u8g2.drawStr((128 - textWidth) / 2, 32, message);
        u8g2.sendBuffer();
        delay(500);
        u8g2.clearBuffer();
        u8g2.sendBuffer();
        delay(500);
    }

    // Display device number
    u8g2.clearBuffer();
    textWidth = u8g2.getStrWidth(deviceMessage);
    u8g2.setCursor((128 - textWidth) / 2, 32);
    u8g2.drawStr((128 - textWidth) / 2, 32, deviceMessage);
    u8g2.sendBuffer();
    delay(1000);

    // Display ready message
    u8g2.clearBuffer();
    textWidth = u8g2.getStrWidth(readyMessage);
    u8g2.setCursor((128 - textWidth) / 2, 32);
    u8g2.drawStr((128 - textWidth) / 2, 32, readyMessage);
    u8g2.sendBuffer();
    delay(1000);
    u8g2.clearBuffer();
}

// ========== READ LIDAR ==========
float readLidarDistance() {
    int16_t distanceCm, flux, temp;
    tflI2C.getData(distanceCm, flux, temp, LIDAR_I2C_ADDR);
    return distanceCm / 100.0;
}

// ========== MAIN LOOP ==========
void loop() {
    unsigned long currentTime = millis();
    float distance = readLidarDistance();

    // Show placeholder display
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_fub30_tr);
    const char* initialDisplay = "00.00";
    int16_t initialTextWidth = u8g2.getStrWidth(initialDisplay);
    u8g2.setCursor((128 - initialTextWidth) / 2, 48);
    u8g2.drawStr((128 - initialTextWidth) / 2, 48, initialDisplay);
    u8g2.sendBuffer();

    if (distance >= 0.0 && distance <= MAX_DETECT_RANGE) {
        float timeDiff = (currentTime - previousTime) / 1000.0;
        if (timeDiff > 0.01) {
            float speedMPerSec = fabs((distance - previousDistance) / timeDiff);
            float speedKmPerHour = speedMPerSec * 3.6;
            previousDistance = distance;
            previousTime = currentTime;

            if (speedKmPerHour >= 1.0 && distance < DISTANCE_THRESHOLD) {
                // Send speed data to Device 1
                SpeedData speedData = {speedKmPerHour, true};
                esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &speedData, sizeof(SpeedData));
                
                if (result != ESP_OK) {
                    Serial.println("Error sending speed data");
                }
            }
        }
    }

    // Check if relay needs to be turned off
    if (relayActive && (currentTime - relayStartTime >= RELAY_ON_DURATION)) {
        digitalWrite(RELAY_PIN, LOW);
        relayActive = false;
        Serial.println("Relay OFF!");
    }

    delay(100);
}

// ========== DISPLAY SPEED ==========
void displaySpeed(float speed) {
    char buffer[10];
    sprintf(buffer, "%.2f", speed);
    int16_t textWidth = u8g2.getStrWidth(buffer);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_fub30_tr);
    u8g2.setCursor((128 - textWidth) / 2, 48);
    u8g2.drawStr((128 - textWidth) / 2, 48, buffer);
    u8g2.sendBuffer();
}

// ========== ACTIVATE RELAY ==========
void activateRelayAndDisplaySpeed(float speed) {
    digitalWrite(RELAY_PIN, HIGH);
    relayActive = true;
    relayStartTime = millis();
    Serial.println("Relay ON!");
    displaySpeedWithAnimation(speed);
}

// ========== ANIMATE SPEED DISPLAY ==========
void displaySpeedWithAnimation(float targetSpeed) {
    float displayedSpeed = 0.0;
    while (displayedSpeed < targetSpeed) {
        displayedSpeed += 0.5;
        if (displayedSpeed > targetSpeed) {
            displayedSpeed = targetSpeed;
        }
        displaySpeed(displayedSpeed);
        delay(20);  // <-- This controls the animation speed of speed increment (ms)
    }

    // Final speed display delay
    displaySpeed(targetSpeed);
    delay(1000);   // <-- Adjust this to hold final speed on screen (ms)
}