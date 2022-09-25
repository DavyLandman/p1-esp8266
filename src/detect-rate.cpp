#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include "secrets.h"

#define IDEAL_PACKET_SIZE 1024

WiFiServer server(23);
WiFiClient current_client;

static size_t baudRateIndex = 0;

static const long baudRates[] = {
    600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000, 256000
};

static char header[128] = {0};

#ifndef SERIAL_BITS
#define SERIAL_BITS SERIAL_8N1
#endif

void setupSerial() {
    if (current_client) {
        sprintf(header, "\n\n*** switching to baud rate: %lu ****\n\n", baudRates[baudRateIndex]);
        current_client.write(header);
    }
    Serial.begin(baudRates[baudRateIndex], SERIAL_BITS, SERIAL_FULL, 1, true);
    Serial.setDebugOutput(false);
}

void setup() {
    setupSerial();

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
    ArduinoOTA.begin();
    
}

#define MIN(a, b) (((a) > (b)) ? (b) : (a))

static unsigned long lastBaudSwitch = 0;
static unsigned long lastUpdate = 0;
static unsigned long shared = 0;
void loop() {
    auto now = millis();
    auto available = Serial.peekAvailable();
    if (available >= IDEAL_PACKET_SIZE || (available > 0 && (now - lastUpdate) > 50)) {
        auto buffer = Serial.peekBuffer();
        if (current_client && current_client.connected()) {
            auto written = current_client.write(buffer, MIN(available, IDEAL_PACKET_SIZE));
            Serial.peekConsume(written);
            shared += written;
            lastUpdate = now;
            if (shared > (IDEAL_PACKET_SIZE * 2) || (now - lastBaudSwitch) > 20000) {
                baudRateIndex++;
                if (baudRateIndex >= (sizeof(baudRates) / sizeof(long))) {
                    baudRateIndex = 0;
                }
                shared = 0;
                lastBaudSwitch = now;
                setupSerial();
            }
        }
        else {
            Serial.peekConsume(0);
        }
    }
    if ((now - lastUpdate > 20000) && current_client) {
        lastUpdate = now;
        current_client.write("\nNo data received\n");
    }
    if (server.hasClient()) {
        current_client.stop();
        current_client = server.available();
    }
    delay(1);
    ArduinoOTA.handle();
}