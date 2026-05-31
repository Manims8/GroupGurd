# Testing

The prototype was tested in indoor and semi-open spaces to evaluate WiFi behavior, display updates, warning alerts, and SOS behavior.

## Test Locations

- College campus
- Hostel
- Residential home

These locations gave a mix of open paths, walls, people moving between devices, and different WiFi reflection/obstruction conditions.

## Tested Functions

| Function | Result |
| --- | --- |
| Real-time distance display | Worked with near-real-time updates |
| Direction arrow display | Worked after compass calibration |
| Threshold warnings | Triggered when devices crossed safe distance limits |
| SOS button | Displayed SOS state and triggered alerts |
| Local TCP communication | Responsive within the D1 access point network |
| Vibration and LED alerts | Worked as clear haptic/visual feedback |

## Observations

RSSI distance was useful for detecting whether a member is close enough or too far, but it was not stable enough for exact meter-by-meter measurement. Obstacles, people, walls, and device orientation caused fluctuation.

The compass made the system more practical because it gave the group leader a direction to check when a warning or SOS occurred. Compass calibration and avoiding magnetic interference are important for consistent direction display.

System latency was low enough for the prototype use case, with alerts appearing quickly after threshold crossing or SOS press.

## Limitations

- RSSI needs environment-specific calibration
- Crowds and obstacles can cause unstable distance values
- Continuous WiFi communication increases power usage
- Breadboard prototypes are not suitable for long-term wearable use
- More real-world festival/market testing is needed

## Outcome

The prototype validates the main idea: a low-cost wearable system can proactively warn a group before members become seriously separated.
