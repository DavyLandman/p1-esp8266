#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "secrets.h"

WiFiServer server(23);
WiFiClient current_client;

#define IDEAL_PACKET_SIZE 1024
void setup() {
    // we use the internal buffer of the serial class and send from that when we can
    // assuming we can send out TCP packets fast enough to not let it overflow
    Serial.setRxBufferSize(IDEAL_PACKET_SIZE * 8);
    Serial.begin(115200, SERIAL_8N1, SERIAL_FULL);
    Serial.setDebugOutput(false);
    // Invert the RX serialport by setting a register value, this way the TX might continue normally allowing the serial monitor to read println's
    USC0(UART0) = USC0(UART0) | BIT(UCRXI);
    Serial.println("Serial port is ready to recieve.");

    WiFi.mode(WIFI_STA);
    Serial.println(WiFi.hostname("ESP-P1-POWER"));
    WiFi.begin(WIFI_ACCESPOINT, WIFI_PASSWORD);
    for (int i = 0 ; i < 20; i++) {
        if (WiFi.status() == WL_CONNECTED) {
            break;
        }
        Serial.println("No WIFI connection yet");
        delay(500);
    }
    Serial.println("Starting server");
    server.begin();
    server.setNoDelay(true);
}

unsigned long lastUpdate = 0;

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
    delay(1);
}