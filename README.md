# Phone Zero

**A working accessibility device for elderly blind users**  

## Project Status: PROOF OF CONCEPT ⚡

Phone Zero **Prototype v0.2.0** successfully validates core GSM communication capabilities using ESP32-WROOM-32 and SIM900A. We have confirmed the ability to place calls, answer calls, send SMS, and receive SMS. This is a **proof of concept stage** - significant development work remains for a complete accessibility device.

**Current Implementation (Prototype v0.2.0):**

- ESP32-WROOM-32 microcontroller with reliable hardware UART2 communication
- SIM900A GSM module for voice calls and SMS (✅ validated)
- Basic button input detection and SMS command processing
- Audio output tested via earphone jack (good quality)
- Single power supply confirmed for both ESP32 and SIM900A

**What We DON'T Have Yet:**

- Microphone and speaker integration
- Audio amplifiers and echo cancellation hardware
- Extended reliability testing (1+ week operation)
- Proper component organization (currently breadboard prototype)
- Complete software implementation (currently < MVP level)

## Hardware Components

**Working Configuration:**

- ESP32-WROOM-32 development board (DoiT ESP32 DevKit v1)
- SIM900A GSM/GPRS module powered via JZK XL4015 DC-DC converter
- JZK XL4015 step-down converter (4-38V to 1.25-36V, max 5A) ⭐ **PROMISING HARDWARE**
- 8 tactile push buttons connected to GPIO pins with internal pull-ups
- Single power supply solution with shared ground

**Connections:**

- SIM900A TX → ESP32 GPIO16 (UART2 RX)
- SIM900A RX → ESP32 GPIO17 (UART2 TX)
- SIM900A GND → ESP32 GND (shared ground)
- Power: JZK XL4015 DC-DC converter → both ESP32 and SIM900A

**Button Mapping:**

- Button 1: GPIO4 | Button 5: GPIO22
- Button 2: GPIO18 | Button 6: GPIO23
- Button 3: GPIO19 | Button 7: GPIO25
- Button 4: GPIO21 | Button 8: GPIO26

## Key Features (Implemented)

- **8 Speed Dial Buttons:** Instant calling with tactile button feedback
- **SMS Programming:** Remote configuration via authorized phone numbers
- **SMS Relay:** Forward messages through the device to other numbers
- **Persistent Storage:** Phone numbers saved to ESP32 flash memory
- **Hardware UART:** Reliable communication using ESP32 UART2
- **Memory Management:** Automatic SMS cleanup for privacy
- **Network Status:** Signal quality monitoring and network registration

## SMS Commands

The device accepts SMS commands from authorized numbers:

- `SET 1 +1234567890` - Program speed dial button 1-8
- `LIST` - Get current speed dial numbers
- `STATUS` - Check device status
- `SMS "message text" +1234567890` - Send SMS to another number

## Project Structure

```text
phone-zero/
├── ESP_GSM/                    # Main firmware (PlatformIO project)
│   ├── src/main.cpp           # Complete Phone Zero implementation
│   └── platformio.ini         # Build configuration
├── docs/                      # Documentation and specifications
│   ├── prototype-v0.2.0/     # Current working prototype documentation
│   ├── boards/               # Hardware pinout diagrams and photos
│   ├── MVP/                  # MVP requirements and milestones
│   └── *.md                  # Technical documentation
└── private/                  # Market research and private notes
```

## Getting Started

### Prerequisites

- PlatformIO IDE or VS Code with PlatformIO extension
- ESP32-WROOM-32 development board
- SIM900A GSM module with 5V/2A power supply
- Valid SIM card with voice and SMS capabilities

### Setup Instructions

1. **Hardware Setup:**

   - Connect ESP32 and SIM900A as described in hardware section
   - Insert activated SIM card into SIM900A module
   - Power SIM900A with independent 5V/2A supply

2. **Firmware Upload:**

   ```bash
   cd ESP_GSM
   pio run --target upload
   ```

3. **Configuration:**
   - Update authorized phone numbers in `main.cpp`
   - Set default speed dial numbers via SMS commands
   - Test button functionality and call features

### Usage

1. **Making Calls:** Press any programmed button (1-8) for instant speed dial
2. **Programming:** Send SMS "SET 1 +1234567890" to program button 1
3. **Status Check:** Send SMS "STATUS" to verify device operation
4. **SMS Relay:** Send SMS "SMS 'Hello!' +1234567890" to forward messages

## Documentation

- **[Prototype v0.2.0](docs/prototype-v0.2.0/) - Current proof of concept with GSM communication validation**
- [Hardware Requirements](docs/hardware-requirements.md) - Component specifications and requirements
- [Phone Specifications](docs/phone-specifications.md) - Core features and accessibility design
- [MVP Requirements](docs/MVP/) - Minimum viable product documentation
- [Board Layouts](docs/boards/) - Hardware pinout diagrams and reference photos

## Future Enhancements

- Physical enclosure design with tactile button layout
- Battery backup system for power outage protection
- Enhanced audio quality with dedicated codec
- Call answering via button press functionality
- Battery status monitoring via SMS notifications
- 3D printed enclosure for production units

## Contributing

Phone Zero is a working accessibility device. Contributions welcome for hardware improvements, firmware enhancements, and documentation updates.

## License

[License information to be added]
