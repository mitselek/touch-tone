# Hardware Setup

This project uses an ESP32 microcontroller connected to a SIM800L GSM modem and an LED.

## Pin Layout

* **SIM800L Modem (UART2):**
  * ESP32 **GPIO 4 (TX)** <-> SIM800L **RX**
  * ESP32 **GPIO 0 (RX)** <-> SIM800L **TX**
  * Ensure common **GND** connection between ESP32 and SIM800L.
  * Provide adequate power supply to the SIM800L (typically requires 2A peak current).
* **LED:**
  * ESP32 **GPIO 5** <-> LED Anode (+)
  * LED Cathode (-) <-> Resistor <-> **GND** (Choose resistor based on LED specifications)

## Connection Diagram

``` ascii
+-------+         +--------+
| ESP32 |         | SIM800L|
|       |         |        |
| GPIO4 |-------->| RX     |
| GPIO0 |<--------| TX     |
| GND   |---------| GND    |
|       |         |        |
| GPIO5 |---+     +--------+
|       |   |
+-------+   |     +-----+
            +---->| LED |---[R]---GND
                  +-----+
```

*Note: GPIO 0 is often used for flashing mode. Ensure your setup allows for both normal operation and flashing.*

---

## How to Build PlatformIO-Based Project

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/platformio/platform-espressif32/archive/develop.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Change directory to the project folder (ESP_GSM)
$ cd /Users/michelek/Documents/GitHub/touch-tone/ESP_GSM

# Build project
$ pio run

# Upload firmware
$ pio run --target upload

# Build specific environment (if multiple are defined in platformio.ini)
$ pio run -e esp32dev

# Upload firmware for the specific environment
$ pio run -e esp32dev --target upload

# Clean build files
$ pio run --target clean
