#pragma once
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Callback types
typedef void (*OnDataReceived)(const uint8_t* mac, const uint8_t* data, int len);

class ESPNowConnect {
public:
    void begin(const uint8_t* peerMAC);
    void send(const uint8_t* data, size_t len);
    void onReceive(OnDataReceived callback);

private:
    uint8_t _peerMAC[6];
    static OnDataReceived _callback;
    //static void _onReceiveInternal(const uint8_t* mac, const uint8_t* data, int len, void* arg);
    static void _onReceiveInternal(const uint8_t* mac, const uint8_t* data, int len);
};