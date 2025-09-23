# Phone Zero

**A working accessibility device for elderly blind users**

## Project Status: FUNCTIONAL ✅

Phone Zero is now a working prototype using ESP32 and SIM900A GSM module, providing 8-button speed dial functionality specifically designed for elderly blind users. The device is operational and tested, featuring tactile button interfaces, SMS programming capabilities, and reliable GSM communication.

**Current Implementation:**
- ESP32-WROOM-32 microcontroller with hardware UART2 communication
- SIM900A GSM module for voice calls and SMS
- 8 programmable speed dial buttons with tactile feedback
- SMS-based number programming and device management
- Persistent storage of phone numbers
- Authorized user system for secure remote configuration

## Hardware Components

**Working Configuration:**
- ESP32-WROOM-32 development board (DoiT ESP32 DevKit v1)
- SIM900A GSM/GPRS module with independent 5V/2A power supply
- 8 tactile push buttons connected to GPIO pins with internal pull-ups
- Shared ground connection between ESP32 and SIM900A

**Connections:**
- SIM900A TX → ESP32 GPIO16 (UART2 RX)
- SIM900A RX → ESP32 GPIO17 (UART2 TX)  
- SIM900A GND → ESP32 GND (shared ground)
- SIM900A VCC → External 5V/2A power supply

**Button Mapping:**
- Button 1: GPIO4  | Button 5: GPIO22
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

```
phone-zero/
├── ESP_GSM/                    # Main firmware (PlatformIO project)
│   ├── src/main.cpp           # Complete Phone Zero implementation
│   └── platformio.ini         # Build configuration
├── docs/                      # Documentation and specifications
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
