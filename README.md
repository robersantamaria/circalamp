# circalamp

Arduinuish circadian lamp

# esp-01 / esp-01s

- To enter serial bootloader:
  - Reset: with power on, or nRESET pin was low and is pulled high, or CH_PD/EN was low and is pulled high.
  - GPIO0 pulled low.

> Both CH_PD/EN and nRESET must be high for the chip to start operating.

> debug logging is at 74880 baud rate.

Resources:

- [A Python-based, open source, platform independent, utility to communicate with the ROM bootloader in Espressif ESP8266 & ESP32 chips](https://github.com/espressif/esptool).
- Flash firmware:
  - https://www.allaboutcircuits.com/projects/flashing-the-ESP-01-firmware-to-SDK-v2.0.0-is-easier-now/
  - With programmer diagram: https://www.allaboutcircuits.com/projects/update-the-firmware-in-your-esp8266-wi-fi-module/
  - 3.0.0 firmware: https://github.com/espressif/ESP8266_NONOS_SDK/releases/tag/v3.0
- [Getting started with AT commands](https://www.instructables.com/id/Getting-Started-With-the-ESP8266-ESP-01/).
- [Tutorial, AT commands, diagram and web server with libraries](https://www.geekstips.com/esp8266-arduino-tutorial-iot-code-example/).

# FTDI USB-to-serial:

- Drivers: https://www.ftdichip.com/Drivers/VCP.htm

# Arduino

- [`millis()` rollover discussion](https://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover).

# General

- Inspiration: [Circadia - sunrise lamp](https://sites.google.com/site/fpgaandco/sunrise)
