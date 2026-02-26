/**
 * =============================================================
 * ESP-NOW Communication Driver
 * File: esp_now_connect.cpp
 * =============================================================
 *
 * DESCRIPTION:
 * This driver provides a simple and generic interface for ESP-NOW
 * communication between two or more ESP32 boards.
 * No shared data structure is required between sender and receiver.
 * Data is sent and received as raw bytes (uint8_t*), allowing you
 * to send any type of data (int, float, String, struct, etc.)
 *
 * REQUIREMENTS:
 * - ESP32 board
 * - PlatformIO with espressif32 platform
 * - WiFi.h and esp_now.h (included in ESP32 Arduino core)
 *
 * =============================================================
 *                *****how to use  this driver****** 
 * =============================================================
 *
 * STEP 1: Get the MAC address of the RECEIVER board
 * -------------------------------------------------
 * Upload this snippet to the receiver ESP32 first:
 *
 *   #include <WiFi.h>
 *   void setup() {
 *     Serial.begin(115200);
 *     Serial.println(WiFi.macAddress());
 *   }
 *   void loop() {}
 *
 * Note the MAC address printed on Serial Monitor (e.g. A4:CF:12:34:56:78)
 *
 * -------------------------------------------------------------
 * STEP 2: SENDER main.cpp
 * -------------------------------------------------------------
 * #include "esp_now_connect.h"
 * ESPNowConnect conn;
 *
 * // Replace with your RECEIVER's MAC address
 * uint8_t receiverMAC[] = {0xA4, 0xCF, 0x12, 0x34, 0x56, 0x78};
 *
 * void setup() {
 *     Serial.begin(115200);
 *     conn.begin(receiverMAC);
 * }
 *
 * void loop() {
 *     // --- Send a String ---
 *     String msg = "Hello!";
 *     conn.send((uint8_t*)msg.c_str(), msg.length());
 *
 *     // --- Send a float ---
 *     float temp = 23.5;
 *     conn.send((uint8_t*)&temp, sizeof(temp));
 *
 *     // --- Send an int ---
 *     int value = 42;
 *     conn.send((uint8_t*)&value, sizeof(value));
 *
 *     // --- Send a struct ---
 *     struct Data { int id; float temp; };
 *     Data d = {1, 23.5};
 *     conn.send((uint8_t*)&d, sizeof(d));
 *
 *     delay(1000);
 * }
 *
 * -------------------------------------------------------------
 * STEP 3: RECEIVER main.cpp
 * -------------------------------------------------------------
 * #include "esp_now_connect.h"
 * ESPNowConnect conn;
 *
 * void handleData(const uint8_t* mac, const uint8_t* data, int len) {
 *
 *     // --- Receive as String ---
 *     Serial.println(String((char*)data, len));
 *
 *     // --- Receive as float ---
 *     float temp;
 *     memcpy(&temp, data, sizeof(temp));
 *     Serial.println(temp);
 *
 *     // --- Receive as int ---
 *     int value;
 *     memcpy(&value, data, sizeof(value));
 *     Serial.println(value);
 *
 *     // --- Receive as struct ---
 *     struct Data { int id; float temp; };
 *     Data d;
 *     memcpy(&d, data, sizeof(d));
 *     Serial.println(d.temp);
 * }
 *
 * void setup() {
 *     Serial.begin(115200);
 *     conn.begin(nullptr);       // Receiver does not need peer MAC
 *     conn.onReceive(handleData); // Register your callback function
 * }
 *
 * void loop() {}
 *
 * =============================================================
 * IMPORTANT RULES
 * =============================================================
 *
 * 1. Sender must know the RECEIVER's MAC address
 * 2. Receiver calls conn.begin(nullptr) — no MAC needed
 * 3. Sender and Receiver must agree on the DATA TYPE being sent
 *    (this is handled only in main.cpp, this driver never changes)
 * 4. Maximum data size per ESP-NOW packet is 250 bytes
 * 5. Both boards must be on the same WiFi channel (default: 0)
 * 6. Always open Serial Monitor at 115200 baud
 *
 * =============================================================
 */
#include "esp_now_connect.h"

// Static callback pointer — holds the user-defined receive handler
OnDataReceived ESPNowConnect::_callback = nullptr;
/**
 * @brief Initializes ESP-NOW and registers the peer device.
 * 
 * Call this function in setup() before sending or receiving data.
 * - For SENDER: pass the receiver's MAC address as argument.
 * - For RECEIVER: pass nullptr, no peer registration needed.
 * 
 * @param peerMAC MAC address of the receiver (6 bytes), or nullptr for receiver mode.
 */
void ESPNowConnect::begin(const uint8_t* peerMAC) {
    WiFi.mode(WIFI_STA);
    
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed!");
        return;
    }

    if (peerMAC != nullptr) {
        memcpy(_peerMAC, peerMAC, 6);
        esp_now_peer_info_t peer = {};
        memcpy(peer.peer_addr, _peerMAC, 6);
        peer.channel = 0;
        peer.encrypt = false;
        esp_now_add_peer(&peer);
    }

    esp_now_register_recv_cb(_onReceiveInternal);
    Serial.println("ESP-NOW initialized successfully!");
}
/**
 * @brief Sends raw data to the registered peer.
 * 
 * Any data type can be sent by casting its address to (uint8_t*).
 * Maximum payload size is 250 bytes (ESP-NOW limitation).
 * 
 * @param data Pointer to the data buffer to send.
 * @param len  Size of the data in bytes (use sizeof()).
 */
void ESPNowConnect::send(const uint8_t* data, size_t len) {
    esp_now_send(_peerMAC, data, len);
}

void ESPNowConnect::onReceive(OnDataReceived callback) {
    _callback = callback;
}

void ESPNowConnect::_onReceiveInternal(const uint8_t* mac, const uint8_t* data, int len) {
    if (_callback) _callback(mac, data, len);
}