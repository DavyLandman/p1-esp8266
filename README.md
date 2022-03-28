# Small ESP serial forwarder for HomeAssistant

## Motivation
I wanted to keep the code as small as possible, so this is a small program that
acts as a serial port forwarder for HomeAssistant.

It reads from P1, and if a client connects to port 23, it forwards everything to
there. You connect HomeAssistant to it and it will do all the parsing.

## Wiring
Wiring is the same as https://github.com/daniel-jong/esp8266_p1meter and also the initial inspiration of using the inverter of the hardware serial.

## How to use
1. In the [include-folder](./include) rename the **'secret_template.h'** to **'secret.h'**, and enter your WiFi credentials.
2. Install to an ESP8266 board with [PlatformIO](https://platformio.org)
3. In HomeAssistant: Add a new [DSMR Slimme Meter integration](https://www.home-assistant.io/integrations/dsmr/) of type Network and enter the IP/Hostname of your ESP8266 and use port 23
