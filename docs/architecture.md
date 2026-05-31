# GroupGuard Architecture

## Device Responsibilities

D1 is the SoftAP and TCP master. It receives one-line CSV packets from D2, D3, and D4, computes approximate distance from RSSI, displays peer state, and sends warnings to every peripheral. It also relays D4 state to D3 and D3 state to D4.

D2 is headless and reports heading, SOS button state, and RSSI. It uses vibration and LED alerts.

D3 displays D1 and D4 in a two-column layout. D3 receives `D1_DATA` from D1 and reports its own state back to D1.

D4 displays D3 and self in the same two-column style. D4 receives `D3_DATA` from D1 and keeps its existing GPIO 27 active-low button wiring.

## Alert Patterns

| Event | Vibration | LED | Duration |
| --- | --- | --- | --- |
| SOS sent | 200 ms on / 200 ms off | Solid on where present | 2000 ms |
| Warning received | 300 ms on / 100 ms off | 150 ms flash where present | 3000 ms |
| D1 warning send | Solid vibration | None | 2000 ms |

## Display Model

Display devices use a split-screen peer box layout:

| Device | Left | Right |
| --- | --- | --- |
| D1 page 1 | D2 | D3 |
| D1 page 2 | D4 full-width | - |
| D3 | D1 | D4 |
| D4 | D3 | D4 self |

Each peer box shows a centered device label, a 32x30 direction arrow, and distance in meters.

## Distance Estimate

D1 estimates distance from RSSI:

```cpp
pow(10, (REF_RSSI - rssi) / (10.0 * PATH_LOSS))
```

This is an approximate proximity indicator and should be tuned in the real environment.

## Data Flow

1. Peripherals connect to D1's `GroupGuard` SoftAP.
2. Peripherals periodically send `D#,heading,sos,rssi` to D1 on port `12345`.
3. D1 updates the sender state using the sender IP from the TCP client.
4. D1 replies to D3 with D1 plus D4 state.
5. D1 replies to D4 with D3 state.
6. Pressing a warning/SOS button sends state through the same channel and triggers local alerts.
