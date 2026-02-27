# ESP32 ESP-NOW Generic Driver

A lightweight and reusable ESP-NOW communication driver for ESP32 boards.
No shared data structure required — only MAC addresses needed to establish communication.

---

## ✨ Features

- Generic raw bytes communication (send any data type)
- No common struct required between sender and receiver
- Simple and clean API — only 3 functions to know
- Professional Doxygen-style comments
- Reusable in any ESP32 PlatformIO project
- Error handling and serial debug messages included

---

## 📋 Requirements

- ESP32 board
- [PlatformIO](https://platformio.org/) with `espressif32` platform
- Arduino framework

---

## 🚀 Quick Start

### Step 1: Get the Receiver MAC Address
Upload this snippet to your receiver ESP32 first:
```cpp
#include <WiFi.h>
void setup() {
  Serial.begin(115200);
  Serial.println(WiFi.macAddress());
}
void loop() {}
```
Note the MAC address printed on Serial Monitor (e.g. `A4:CF:12:34:56:78`)

---

### Step 2: Copy the Driver Files
Copy these two files into your project `src/` folder:
```
src/
├── main.cpp
├── esp_now_connect.h
└── esp_now_connect.cpp
```

---

### Step 3: Sender `main.cpp`
```cpp
#include <Arduino.h>
#include "esp_now_connect.h"

ESPNowConnect conn;

// Replace with your receiver's MAC address
uint8_t receiverMAC[] = {0xA4, 0xCF, 0x12, 0x34, 0x56, 0x78};

void setup() {
    Serial.begin(115200);
    conn.begin(receiverMAC);
}

void loop() {
    float temperature = 23.5;
    conn.send((uint8_t*)&temperature, sizeof(temperature));
    delay(1000);
}
```

---

### Step 4: Receiver `main.cpp`
```cpp
#include <Arduino.h>
#include "esp_now_connect.h"

ESPNowConnect conn;

void handleData(const uint8_t* mac, const uint8_t* data, int len) {
    float temperature;
    memcpy(&temperature, data, sizeof(temperature));
    Serial.print("Temperature received: ");
    Serial.print(temperature);
    Serial.println(" °C");
}

void setup() {
    Serial.begin(115200);
    conn.begin(nullptr);
    conn.onReceive(handleData);
}

void loop() {}
```

---

## 📦 Sending Different Data Types

### String
```cpp
// Sender
String msg = "Hello ESP32!";
conn.send((uint8_t*)msg.c_str(), msg.length());

// Receiver
String msg = String((char*)data, len);
Serial.println(msg);
```

### Integer
```cpp
// Sender
int value = 42;
conn.send((uint8_t*)&value, sizeof(value));

// Receiver
int value;
memcpy(&value, data, sizeof(value));
Serial.println(value);
```

### Struct
```cpp
// Sender
struct SensorData { float temperature; int humidity; };
SensorData s = {23.5, 60};
conn.send((uint8_t*)&s, sizeof(s));

// Receiver
struct SensorData { float temperature; int humidity; };
SensorData s;
memcpy(&s, data, sizeof(s));
Serial.println(s.temperature);
```

---

## ⚠️ Important Rules

| Rule | Detail |
|------|--------|
| Max packet size | 250 bytes (ESP-NOW limitation) |
| Sender | Must know receiver MAC address |
| Receiver | Call `conn.begin(nullptr)` |
| Baud rate | Always use `115200` |
| Data type | Sender and receiver must agree on the type being sent |

---

## 🔧 API Reference

### `void begin(const uint8_t* peerMAC)`
Initializes ESP-NOW. Pass receiver MAC for sender, `nullptr` for receiver.

### `void send(const uint8_t* data, size_t len)`
Sends raw bytes to the registered peer. Max 250 bytes.

### `void onReceive(OnDataReceived callback)`
Registers a callback function to handle incoming data.

---

## 👤 Author

**Chaanbi**
- GitHub: [@Chaanbi-tns](https://github.com/Chaanbi-tns)

---

## 📄 License

MIT License — free to use, modify, and share.
