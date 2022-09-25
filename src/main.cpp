#include <Arduino.h>
#ifdef OTA
#include <ArduinoOTA.h>
#endif
#include <ESP8266WiFi.h>
#include "secrets.h"

#ifndef SERIAL_RATE
#define SERIAL_RATE 115200
#endif

#ifndef SERIAL_BITS
#define SERIAL_BITS SERIAL_8N1
#endif

WiFiServer server(23);
WiFiClient current_client;

#define IDEAL_PACKET_SIZE 1024
void setup() {
    // we start a serial, but invert it
    Serial.begin(SERIAL_RATE, SERIAL_BITS, SERIAL_RX_ONLY, 1, true);
    // we use the internal buffer of the serial class and send from that when we can
    // assuming we can send out TCP packets fast enough to not let it overflow
    Serial.setRxBufferSize(IDEAL_PACKET_SIZE * 8);
    Serial.setDebugOutput(false);

    String hostname = "ESP-P1-POWER-" + WiFi.macAddress(); // allow for multiple esp-p1-power on the network
    hostname.replace(":", "");
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.hostname(hostname.c_str());
    WiFi.begin(WIFI_ACCESPOINT, WIFI_PASSWORD);
    for (int i = 0 ; i < 20; i++) {
        if (WiFi.status() == WL_CONNECTED) {
            break;
        }
        Serial.println("No WIFI connection yet");
        delay(500);
    }
    server.begin();
    server.setNoDelay(true);

#ifdef OTA
    ArduinoOTA.setPassword("MONITOR_POWER_42");
    ArduinoOTA.setHostname(hostname.c_str());
    ArduinoOTA.begin();
#endif
}

unsigned long lastUpdate = 0;
#ifdef OTA
unsigned long lastOTACheck = 0;
#endif

void loop() {
    auto now = millis();

    auto available = Serial.peekAvailable();
    if (available >= IDEAL_PACKET_SIZE || (available > 0 && (now - lastUpdate) > 50)) {
        auto buffer = Serial.peekBuffer();
        if (current_client && current_client.connected()) {
            auto written = current_client.write(buffer, available);
            Serial.peekConsume(written);
            lastUpdate = now;
        }
        else {
            Serial.peekConsume(0);
        }
    }
    if (server.hasClient()) {
        current_client.stop();
        current_client = server.available();
    }
#ifdef OTA
    if (now - lastOTACheck > 1000) {
        ArduinoOTA.handle();
        lastOTACheck = now;
    }
    else {
        delay(10);
    }
#else
    delay(10);
#endif
}