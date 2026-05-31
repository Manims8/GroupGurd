# Hardware

## Core Components

| Component | Purpose |
| --- | --- |
| ESP32-WROOM-32 | Main microcontroller, WiFi networking, GPIO control |
| QMC5883L/HMC5883L compass | Heading and direction sensing over I2C |
| 0.96 inch OLED display | Distance, direction, warning, and SOS display |
| Vibration motor | Haptic alert feedback |
| Push button | SOS or warning trigger |
| LED | Visual alert feedback where present |
| Li-ion/LiPo battery | Portable wearable power source |
| TP4056 charging module | Battery charging module |
| Wrist strap/enclosure | Wearable housing |

## Pin Mapping

| Device | Button | Vibration | LED | I2C | Display |
| --- | --- | --- | --- | --- | --- |
| D1 Master | GPIO 15, pulldown, active-high | GPIO 5 | None | SDA 21, SCL 22 | I2C SSD1306 |
| D2 Peripheral | GPIO 15, pulldown, active-high | GPIO 5 | GPIO 25 | SDA 21, SCL 22 | None |
| D3 Display | GPIO 15, pulldown, active-high | GPIO 5 | None | SDA 21, SCL 22 | I2C SSD1306 |
| D4 Display | GPIO 27, pullup, active-low | GPIO 26 | GPIO 25 | SDA 21, SCL 22 | SPI SSD1306, DC 5, RST 4 |

D4 intentionally uses an active-low button connected to ground. Do not change this wiring in firmware unless the physical circuit is changed.

## Circuit Diagram

![Circuit diagram](../images/circuit-diagrams/circuit%20diagram.png)

## 3D Models

The repository includes two STL models:

- [`with_display.stl`](../3D-designs/with_display.stl)
- [`without_display.stl`](../3D-designs/without_display.stl)

These are intended for wristband enclosures with and without OLED display cutouts.
