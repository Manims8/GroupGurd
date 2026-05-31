# GroupGuard - Code Cleanup & Restructuring

Comprehensive refactoring of all 4 ESP32 device codes to unify WiFi credentials, display interfaces, SOS/alert behavior, code structure, and create a clean GitHub repository.

## User Review Required

> [!IMPORTANT]
> **WiFi Credentials Change**: D1 currently creates a SoftAP with legacy credentials. This will be changed to SSID `"GroupGuard"` / password `"Groupguard@0826"`. D1 will NOT show these credentials on screen during startup (as requested: "Don't show while starting in D1"). D2, D3, D4 already connect to this AP — their credentials will be updated to match.

> [!WARNING]
> **D4 Display Change**: D4 currently shows D1 info (distance/direction to D1 Master). This will be changed to show **D3 info only** (distance/direction to D3), which means D1 must relay D3 data to D4, similar to how it already relays D4 data to D3.

## Open Questions

> [!IMPORTANT]
> **D3 compass averaging bug**: D3 previously read one compass sample and divided by 10. This is fixed to use the same 10-sample averaging as the other devices.

> [!IMPORTANT]
> **D4 button logic**: D4 uses `INPUT_PULLUP` with `digitalRead == LOW` for SOS (active-low button to GND). D2/D3 use `INPUT_PULLDOWN` with `digitalRead == HIGH` (active-high). These are hardware-specific and will be left as-is. Confirm this is intentional.

## Proposed Changes

### 1. Repository Structure (New Layout)

The flat structure `D1_code/D1_code.ino`, `D2_code/D2_code.ino`, etc. will be reorganized into a clean GitHub-ready repo:

```
project_ws/
├── README.md                          [NEW]
├── LICENSE                            [NEW]
├── docs/
│   └── architecture.md                [NEW]
├── firmware/
│   ├── D1_Master/
│   │   └── D1_Master.ino              [NEW - replaces D1_code/D1_code.ino]
│   ├── D2_Peripheral/
│   │   └── D2_Peripheral.ino          [NEW - replaces D2_code/D2_code.ino]
│   ├── D3_Display/
│   │   └── D3_Display.ino             [NEW - replaces D3_code/D3_code.ino]
│   └── D4_Display/
│       └── D4_Display.ino             [NEW - replaces D4_code/D4_code.ino]
└── .gitignore                         [NEW]
```

The old `D1_code/`, `D2_code/`, `D3_code/`, `D4_code/` folders will be removed.

---

### 2. WiFi Credential Updates

#### [MODIFY] All device files

| Device | Current SSID | Current Password | New SSID | New Password |
|--------|-------------|-----------------|----------|-------------|
| D1 (AP) | legacy SSID | legacy password | `"GroupGuard"` | `"Groupguard@0826"` |
| D2 | `"GroupGuard"` | `"Groupguard@0826"` | ✅ Already correct | ✅ Already correct |
| D3 | legacy SSID | legacy password | `"GroupGuard"` | `"Groupguard@0826"` |
| D4 | legacy SSID | legacy password | `"GroupGuard"` | `"Groupguard@0826"` |

**D1 startup**: Remove the screen showing hotspot credentials. Replace with a clean "Initializing..." splash instead.

---

### 3. D4 Display — Show D3 Instead of D1

#### [MODIFY] D1_Master.ino
- When D1 receives data from D3, it must relay D3's heading/distance info to D4 (similar to how D1 already relays D4 info to D3)
- Add a new function `sendDataToD4()` that sends: `D3_DATA,<d3Heading>,<d3Distance>,<d3Sos>,<d3Active>`

#### [MODIFY] D4_Display.ino
- Replace `d1_state` with `d3_state` tracking struct
- Parse `D3_DATA` packets instead of `D1_DATA` packets
- Display "D3" label and D3's distance/direction on screen

---

### 4. Common Display Interface (Unified Layout)

Currently each display device looks different:
- **D1**: Split-screen with 2 peer boxes (D2+D3 on page 0, D4+D1-info on page 1)
- **D3**: Split-screen with 2 peer boxes (D1+D4)
- **D4**: Full-screen single panel showing D1 info (completely different layout)

**Unified approach**: All devices with displays (D1, D3, D4) will use the **same split-screen two-column layout**:

| Device | Left Column | Right Column | Page Toggle |
|--------|-------------|-------------|-------------|
| D1 | D2 info | D3 info | Page 2: D4 info + D1 Self info |
| D3 | D1 info | D4 info | Single page (2 peers) |
| D4 | D3 info | Self info | Single page |

Each peer box uses the same `drawPeerBox()` function:
- Device name at top (centered)
- Direction arrow bitmap (centered)
- Distance in meters (centered, bottom)

**Arrow bitmaps**: D1 uses 28×28 arrows while D3/D4 use 32×30 arrows. I will **standardize all to 32×30** bitmaps (the larger ones used by D3/D4). This means D1's bitmaps will be replaced.

---

### 5. Unified SOS/Alert System

Currently the alert behavior is inconsistent:

| Device | Warning Received | SOS Triggered | LED Behavior | Vibration |
|--------|-----------------|---------------|-------------|-----------|
| D2 | Pulse vibe 300/100ms for 3s, LED solid 3s | Pulse vibe 200/200ms for 1.5s, LED flash 100/100ms | Complex state machine | Complex state machine |
| D3 | Solid vibe 3s | Solid vibe 1s | No LED | Simple on/off |
| D4 | Solid vibe 3s | Solid vibe 1s | Heartbeat blink / rapid flash on warning | Simple on/off, LED managed separately |
| D1 | N/A (sends warnings) | Solid vibe 2s | N/A | Simple on/off |

**Unified Alert Pattern** (all devices with vibe motor + LED):

| Event | Vibration | LED | Duration | Display |
|-------|-----------|-----|----------|---------|
| **SOS Sent** (own button) | Pulse: 200ms ON / 200ms OFF | Solid ON | 2000ms | Blinking "SOS" text + "Sending Alert" |
| **SOS Received** (from peer) | Pulse: 300ms ON / 100ms OFF | Flash: 100ms ON / 100ms OFF | 3000ms | Blinking "SOS [device]" text |
| **Warning from D1** | Pulse: 300ms ON / 100ms OFF | Flash: 150ms ON / 150ms OFF | 3000ms | Triangle warning icon + "D1 ALERT" |
| **D1 Sending Warning** | Solid ON | N/A (D1 has no LED) | 2000ms | "SENDING ALERT" text |

All devices will use the **same `triggerAlert()` / `handleAlerts()` state machine** (currently only D2 has this). D3, D4, and D1 will adopt this unified approach.

---

### 6. Unified Code Structure

All `.ino` files will follow the **same section ordering and naming convention**:

```
1. Header Comment Block (role, hardware, connections)
2. LIBRARIES
3. NETWORK CONFIGURATION
4. HARDWARE CONFIGURATION
5. DISPLAY ASSETS (logo bitmap, arrow bitmaps) — if applicable
6. DATA STRUCTURES (DeviceState struct)
7. GLOBAL STATE VARIABLES
8. ALERT STATE MACHINE (triggerAlert / handleAlerts)
9. FUNCTION PROTOTYPES
10. setup()
11. loop()
12. NETWORK FUNCTIONS (connectToWiFi, handleIncomingData, sendDataToMaster, etc.)
13. COMPASS FUNCTIONS (readCompassHeading)
14. DISPLAY FUNCTIONS (updateDisplay, drawPeerBox, drawSOSOverlay, drawRangeWarning)
15. UTILITY FUNCTIONS (normalizeAngle, getArrowBitmap, calculateDistance)
```

Naming conventions:
- Constants: `UPPER_SNAKE_CASE`
- Variables: `camelCase`
- Functions: `camelCase`
- Struct types: `PascalCase`

---

### 7. Code Quality Improvements

- **Consistent compass averaging**: All devices use the same 10-sample averaging pattern via a dedicated `readCompassHeading()` function
- **Fix D3 bug**: Change the single-sample compass loop to the shared 10-sample averaging loop.
- **Consistent string buffer sizes**: Use `char[64]` everywhere for data packets
- **Remove magic numbers**: Replace all inline constants with named defines
- **Consistent Serial debug format**: All devices print in format `"[D#] message"`
- **WiFi reconnection**: D3/D4 already have reconnection logic; verify it's consistent

---

### 8. New Files

#### [NEW] README.md
Professional README with:
- Project title & description
- System architecture diagram (Mermaid)
- Hardware requirements per device
- Pin mapping table
- WiFi configuration
- How to flash each device
- Communication protocol documentation

#### [NEW] .gitignore
Standard Arduino/PlatformIO gitignore

#### [NEW] docs/architecture.md
Detailed technical documentation

#### [NEW] LICENSE
MIT License

---

## Verification Plan

### Automated Checks
- Verify all files have consistent WiFi credentials (`"GroupGuard"` / `"Groupguard@0826"`)
- Verify D1 does NOT display credentials on startup screen
- Verify D4 code references `d3_state` instead of `d1_state`
- Verify all devices have the unified alert state machine
- Grep for old SSID to ensure it's fully removed
- Verify consistent code structure across all 4 files

### Manual Verification
- User to flash all 4 devices and verify:
  - WiFi connection works with new credentials
  - D4 display shows D3 info instead of D1
  - SOS alarm behavior is consistent across devices
  - Display layout matches across D1/D3/D4
