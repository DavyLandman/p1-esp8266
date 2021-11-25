# Small ESP serial forwarder for HomeAssistent

I wanted to keep the code as small as possible, so this is a small program that
acts as a serial port forwarder for HomeAssistent.

It reads from P1, and if a client connects to port 23, it forwards everything to
there. You connect HomeAssistent to it and it will do all the parsing.



Wiring the same as https://github.com/daniel-jong/esp8266_p1meter and also the initial inspiration of using the inverter of the hardware serial.

In the include-folder rename the **'secret_template.h'** to **'secret.h'**, and enter your WiFi credentials.