# Phone Zero - AI Coding Agent Instructions

## Project Overview

Phone Zero is an **accessibility-focused bedside phone** for elderly blind users. This is currently at **proof-of-concept stage** (Prototype v0.2.0) with validated GSM communication but significant development work remaining. The device features large tactile buttons instead of a screen, SMS-programmable speed dial, and always-on power design.

**Target User:** Elderly blind users requiring a stationary bedside phone with tactile interface.

## Architecture & Key Components

### Hardware Stack (Prototype v0.2.0)
- **ESP32-WROOM-32** (DoiT ESP32 DevKit v1) - Main microcontroller
- **SIM900A GSM Module** - Voice calls and SMS (2G, will upgrade to 4G in future)
- **JZK XL4015 DC-DC Converter** - Single power supply for both ESP32 and SIM900A (validated)
- **8 Tactile Buttons** - Speed dial interface (GPIO4, 18, 19, 21, 22, 23, 25, 26)

### Critical Hardware Detail: SIM900A Auto Power-Up
The SIM900A requires a **hardware modification** for automatic startup:
1. Solder bridge R13 on SIM900A board (two tiny pads right of bottom red headers)
2. Connect SIM900A D9 pin to ESP32 GPIO27
3. Software control via `digitalWrite(SIM900_POWER_PIN, HIGH/LOW)` sequence
See [main.cpp](ESP_GSM/src/main.cpp#L113-L117) for implementation.

### Communication Architecture
- **ESP32↔SIM900A:** Hardware UART2 (GPIO16=RX, GPIO17=TX) at 9600 baud
  - **Why Hardware UART:** Previous Arduino SoftwareSerial was unreliable; ESP32 UART2 solves this
  - **Power:** Independent 5V/2A supply for SIM900A, shared GND
- **Button Input:** Internal pull-up resistors, LOW = pressed
- **Persistent Storage:** ESP32 Preferences library stores speed dial numbers in flash

## Development Workflow

### Build & Flash
```bash
cd ESP_GSM
pio run                    # Compile only
pio run --target upload    # Upload to ESP32
pio device monitor         # Serial monitor at 115200 baud
```

### Testing Commands (via Serial Monitor)
- `SMSCHECK` - Check SMS memory status
- `SMSCLEAR` - Delete all SMS messages
- `AT+CSQ` - Check signal quality
- `AT+CREG?` - Check network registration
- Any other AT command forwards directly to SIM900A

### SMS Programming Protocol
From authorized numbers only (defined in [main.cpp](ESP_GSM/src/main.cpp#L68-L71)):
- `SET 1 +1234567890` - Program button 1-8
- `LIST` - Get current speed dial numbers  
- `STATUS` - Check device status
- `SMS "message" +1234567890` - Send SMS relay

**Security:** Only numbers in `authorizedNumbers[]` array can program the device.

## Critical Code Patterns

### Button Handling with Debouncing
All button reads use internal pull-ups (HIGH=unpressed, LOW=pressed). See [checkButtons()](ESP_GSM/src/main.cpp#L219-L259) for debounce implementation with 50ms delay.

**Pattern:** Wait for button release before re-enabling detection to avoid multiple triggers.

### Persistent Storage Pattern
```cpp
preferences.begin("phone-zero", false);  // Namespace
preferences.putString("button1", number);
String num = preferences.getString("button1", "");
```
**Convention:** Keys are "button1" through "button8", default empty string.

### AT Command Error Handling
Currently **minimal error handling** - this is a proof-of-concept. When implementing error recovery:
- SIM900A responses contain "OK" or "ERROR"
- Network failures require retry logic
- SMS memory fills up (auto-cleanup every 10 minutes implemented)

## What's Missing (Below MVP)

**Audio System:**
- No microphone/speaker integration yet (only tested via SIM900A earphone jack)
- No audio amplifiers or echo cancellation hardware
- Considering Cirrus Logic CS48L32-CWZR for audio processing

**Reliability:**
- No long-term power consumption testing (need 1+ week validation)
- No thermal testing for continuous operation
- Breadboard setup needs PCB or proper organization

**Software Gaps:**
- Limited call flow management
- No comprehensive error recovery
- No battery monitoring (future: SMS notifications at 100%, 75%, 50%, 25%, 10%, 5%)

## Future Roadmap (Post-MVP)

1. **Hardware Evolution:** Migrate to ESP32-S3 + SIM7670G/A7670 (4G Cat-1) integrated boards
   - Target: Waveshare ESP32-S3 SIM7670G or Makerfabs ESP32S3 4G LTE CAT1
2. **Audio:** Add microphone, speaker, amplifiers, echo cancellation
3. **Power:** Integrate large LiPo battery with SMS status updates
4. **Accessibility:** Vibration feedback, movement sensor for call answering

## Documentation Structure

- [README.md](README.md) - Current status and quick start
- [docs/phone-specifications.md](docs/phone-specifications.md) - Full product vision
- [docs/MVP/mvp-requirements.md](docs/MVP/mvp-requirements.md) - Minimum viable product scope
- [docs/prototype-v0.2.0/README.md](docs/prototype-v0.2.0/README.md) - Current prototype details
- [docs/hardware-requirements.md](docs/hardware-requirements.md) - Component selection rationale
- `private/` - Market research (not for AI reference)

## Code Style & Conventions

- **Pin Definitions:** Use named constants (`#define BUTTON_1 4`) at top of file
- **Serial Debug:** Verbose logging to Serial (115200 baud) for development
- **Comments:** Explain hardware connections and "why" behind decisions
- **AT Commands:** Always log sent commands and responses for debugging
- **Array Indexing:** Buttons are 1-8 for users, 0-7 in code arrays

## When Making Changes

1. **Hardware Changes:** Update pin definitions in comments and code together
2. **AT Commands:** Reference SIM900A AT command manual, test responses
3. **Flash Storage:** Changes to Preferences schema need migration path
4. **Power-Critical Code:** Consider impact on long-term reliability/power consumption
5. **Accessibility:** Test changes don't require visual feedback for operation
