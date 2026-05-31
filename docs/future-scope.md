# Future Scope

## PCB And Enclosure

Move from breadboard wiring to a custom PCB and compact enclosure. This will make the wristband smaller, more durable, and easier to wear.

## Power Optimization

Improve battery life by:

- Using ESP32 light sleep or modem sleep
- Reducing send frequency when members are close
- Increasing send frequency only when distance grows
- Turning off display or reducing refresh rate when idle

## Better Positioning

RSSI is useful for approximate threshold alerts, but not precise location. Future versions could explore:

- UWB for centimeter-level ranging
- BLE Mesh for larger low-power groups
- ESP-NOW for lightweight peer-to-peer ESP32 communication
- GPS for outdoor fallback tracking

## Mobile App

A companion app could help with:

- Naming devices
- Adjusting distance thresholds
- Viewing status of all bands
- Receiving phone notifications
- Saving test/calibration logs

## Scalability

The current system uses a star topology around D1. Larger groups may need a mesh or multi-leader architecture where family heads relay data to the main group leader.

## Field Testing

More testing should be done in:

- Busy markets
- Temples during rush hours
- Festivals
- Tourist locations
- Mixed indoor/outdoor routes

This would help tune RSSI thresholds and validate the system under realistic crowd movement.
