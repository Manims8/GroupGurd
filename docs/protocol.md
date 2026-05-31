# Communication Protocol

GroupGuard uses a local TCP network. D1 creates the WiFi access point and runs the central server on port `12345`. Peripherals run a server on port `54321` for warning and relay packets.

## Network

| Setting | Value |
| --- | --- |
| D1 SoftAP SSID | `GroupGuard` |
| D1 SoftAP IP | `192.168.4.1` |
| D1 server port | `12345` |
| Peripheral server port | `54321` |

Change the default password before using the project outside a controlled prototype environment.

## Peripheral To D1

Each peripheral periodically sends:

```text
D#,<heading>,<sos>,<rssi>
```

Example:

```text
D3,142,0,-61
```

Fields:

| Field | Meaning |
| --- | --- |
| `D#` | Device name, such as `D2`, `D3`, or `D4` |
| `heading` | Compass heading in degrees |
| `sos` | `1` if SOS is active, otherwise `0` |
| `rssi` | WiFi RSSI reading in dBm |

## D1 To D3

D1 sends D1 state plus D4 relay data to D3:

```text
D1_DATA,<d1Heading>,<d3Distance>,<d4Heading>,<d4Distance>,<d4Sos>,<d4Active>
```

## D1 To D4

D1 sends D3 relay data to D4:

```text
D3_DATA,<d3Heading>,<d3Distance>,<d3Sos>,<d3Active>
```

## Warning Packet

Warnings use a simple command:

```text
WARN
```

When received, peripherals trigger their vibration/LED warning pattern and display devices show a warning screen.

## Distance Formula

D1 estimates distance from RSSI:

```cpp
pow(10, (REF_RSSI - rssi) / (10.0 * PATH_LOSS))
```

RSSI distance is approximate and should be treated as a threshold signal, not exact measurement.
