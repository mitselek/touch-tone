# Phone Zero Prototype v0.2.0

**Status:** 🔧 **PROOF OF CONCEPT - GSM COMMUNICATION VALIDATED**  
**Date:** September 2025  
**Hardware Platform:** ESP32-WROOM-32 + SIM900A GSM Module

## Overview

Prototype v0.2.0 represents our second hardware iteration focused on validating GSM communication capabilities. This is **NOT a complete working prototype** but rather a proof of concept that confirms we can reliably place calls, answer calls, send SMS, and receive SMS using ESP32 hardware UART communication with the SIM900A module.

## Hardware Components

### Main Components

- **ESP32-WROOM-32 Development Board** (DoiT ESP32 DevKit v1)
  - 4MB Flash memory
  - WiFi/Bluetooth capability (unused in this application)
  - Hardware UART2 for reliable communication
- **SIM900A GSM/GPRS Module**

  - Quad-band GSM connectivity
  - Voice calling and SMS capabilities
  - Powered via shared DC-DC converter

- **JZK XL4015 DC-DC Step-Down Converter** ⭐ **PROMISING HARDWARE**
  - Max 5A DC-DC adjustable step-down converter
  - Input: 4-38V, Output: 1.25-36V
  - High power LED constant current driver capability
  - **Successfully powers both ESP32 and SIM900A from single PSU**
  - Eliminates need for separate power supplies

### Tactile Interface

- **8 Tactile Push Buttons** for speed dial
- Connected to GPIO pins with internal pull-up resistors
- Instant calling functionality with button press

## Wiring Configuration

### ESP32 to SIM900A Communication

```text
SIM900A TX  → ESP32 GPIO16 (UART2 RX, D16)
SIM900A RX  → ESP32 GPIO17 (UART2 TX, D17)
SIM900A GND → ESP32 GND (shared ground)
SIM900A VCC → External 5V/2A power supply
```

### Button Mapping

```text
Button 1: GPIO4  (D4)  | Button 5: GPIO22 (D22)
Button 2: GPIO18 (D18) | Button 6: GPIO23 (D23)
Button 3: GPIO19 (D19) | Button 7: GPIO25 (D25)
Button 4: GPIO21 (D21) | Button 8: GPIO26 (D26)
```

## What We've Validated ✅

### GSM Communication Core Functions

- **Call Placement:** Can initiate voice calls to any number
- **Call Reception:** Can detect and answer incoming calls
- **SMS Transmission:** Can send SMS messages reliably
- **SMS Reception:** Can receive and parse SMS messages
- **Network Registration:** Successful connection to cellular network
- **Hardware UART:** Reliable ESP32↔SIM900A communication

### Basic Software Framework

- SMS command parsing (SET, LIST, STATUS commands)
- Persistent storage using ESP32 Preferences
- Button input detection and debouncing
- Authorized user verification system

### Hardware Integration Success

- **ESP32-SIM900A Communication:** Hardware UART2 eliminates previous Arduino SoftwareSerial issues
- **Single Power Supply:** ⭐ JZK XL4015 DC-DC converter successfully powers both ESP32 and SIM900A modules
- **Audio Output:** Tested SIM900A audio jack with earphones - sound quality is good
- **Power Efficiency:** Consolidated power architecture reduces complexity and potential failure points

## What We DON'T Have Yet ❌

### Software Limitations

- **Below MVP level:** Current software is proof-of-concept only
- **No complete call flow:** Can place/answer calls but limited integration
- **Basic SMS features:** Command parsing works but needs enhancement
- **No error recovery:** Limited handling of network/hardware failures
- **No audio processing:** No software audio management or echo cancellation

### Hardware Gaps

- **No Microphone:** Not yet integrated for voice input
- **No Speaker:** No dedicated speaker for audio output (only tested via earphone jack)
- **Breadboard Setup:** Components loosely connected, needs proper organization
- **No Amplifiers:** Missing microphone preamp and speaker amplifier circuits
- **No Audio Processing:** Missing echo cancellation hardware (considering CS48L32-CWZR)

### Long-term Reliability Unknown

- **Power Consumption:** Untested for extended operation (need 1 week+ test)
- **Thermal Performance:** No thermal testing of continuous operation
- **Component Durability:** No stress testing of connections/components

## Promising Hardware Components ⭐

### Power Management

- **JZK XL4015 DC-DC Step-Down Converter**
  - Model: XL4015E1 Step Down Power Supply Voltage Regulator Module
  - Specifications: Max 5A, Input 4-38V, Output 1.25-36V
  - Features: High power LED constant current driver capability
  - **Validated:** Successfully powers both ESP32-WROOM-32 and SIM900A from single input
  - **Advantage:** Eliminates complexity of dual power supply system
  - **Consideration for v0.3.0:** Primary power management solution

### Audio Processing (Under Evaluation)

- **Cirrus Logic CS48L32-CWZR**
  - Echo cancellation and audio processing capabilities
  - Potential solution for audio feedback issues
  - **Status:** Research phase, not yet tested

## Critical Unsolved Problems 🔍

### Audio System Architecture

- **Echo Cancellation:** Need hardware solution like Cirrus Logic CS48L32-CWZR
- **Microphone Interface:** Requires proper preamp and ADC integration
- **Speaker Interface:** Needs power amplifier for adequate volume
- **Audio Routing:** Software control of audio paths (mic/speaker/earphone)

### Physical Design

- **Form Factor:** Casing design depends on final component selection
- **Component Layout:** Need larger breadboard or PCB design for organization
- **Connector Strategy:** Microphone and speaker connection methods undefined
- **Accessibility Design:** Button layout and tactile feedback not finalized

### System Integration

- **Power Management:** Long-term power consumption and battery backup
- **Thermal Management:** Heat dissipation for continuous operation
- **Reliability Testing:** Extended operation validation needed
- **Automatic Power-Up:** Currently requires manual SIM900A power button press - needs automatic startup when power applied

## Technical Achievements

### Hardware UART Success

This prototype overcomes the communication limitations experienced with Arduino platforms by using ESP32's hardware UART2 instead of software serial. This provides:

- **Reliable Communication:** No data corruption or timing issues
- **Full Duplex Operation:** Simultaneous send/receive capability
- **Buffer Management:** Hardware-level buffering prevents data loss

### Power Architecture

- **Single Power Supply Solution:** JZK XL4015 DC-DC converter successfully powers both ESP32 and SIM900A
- **Shared Ground:** Common ground connection ensures reliable communication
- **Power Consolidation:** Eliminates need for separate ESP32 USB power and SIM900A dedicated adapter
- **Voltage Regulation:** Adjustable output (1.25-36V) allows optimal power delivery to both modules

### Memory Management

- **ESP32 Preferences:** Non-volatile storage for speed dial numbers
- **SMS Cleanup:** Automatic memory management prevents overflow
- **Privacy Protection:** Messages deleted immediately after processing

## Development Evolution

### From Previous Iterations

- **v0.1.x:** Arduino Uno/Nano with SoftwareSerial (communication issues)
- **v0.2.0:** ESP32-WROOM-32 with Hardware UART2 (✅ SUCCESS)

### Key Improvements in v0.2.0

1. **Hardware UART2** replaced unreliable SoftwareSerial
2. **ESP32 Preferences** replaced Arduino EEPROM for better persistence
3. **Enhanced GPIO Selection** avoiding ESP32 strapping pins
4. **Comprehensive SMS Features** including relay functionality
5. **Robust Error Handling** with automatic recovery mechanisms

## Photo Documentation

This prototype is documented with photos showing:

- Complete hardware setup and wiring
- ESP32 and SIM900A module placement
- Button connections and layout
- Power supply configuration
- Working prototype in operation

**Photo Documentation:**

- [IMG_6212.jpg](IMG_6212.jpg) - Overview of complete setup
- [IMG_6213.jpg](IMG_6213.jpg) - ESP32-WROOM-32 wiring 1/4
- [IMG_6214.jpg](IMG_6214.jpg) - 2/4
- [IMG_6215.jpg](IMG_6215.jpg) - 3/4
- [IMG_6216.jpg](IMG_6216.jpg) - 4/4
- [IMG_6217.jpg](IMG_6217.jpg) - UART2 connections (GPIO16/17)
- [IMG_6218.jpg](IMG_6218.jpg) - SIM900A GSM module
- [IMG_6219.jpg](IMG_6219.jpg) - Power the SIM900A module
- [IMG_6220.jpg](IMG_6220.jpg) - Power the SIM900A module
- [IMG_6221.jpg](IMG_6221.jpg) - Power the SIM900A module
- [IMG_6222.jpg](IMG_6222.jpg) - Power supply  
- [tidy_up.jpg](tidy_up.jpg) - **Organized layout with ESP32 on breadboard**  

## Firmware

**Location:** `/ESP_GSM/src/main.cpp`  
**Platform:** PlatformIO with Arduino framework  
**Key Libraries:**

- ESP32 Preferences for persistent storage
- Hardware Serial (UART2) for SIM900A communication

## Testing Status

### ✅ Verified Features

- [x] Button press detection and debouncing
- [x] Speed dial calling functionality
- [x] SMS reception and parsing
- [x] SMS command processing
- [x] Number programming via SMS
- [x] SMS relay functionality
- [x] Persistent storage of phone numbers
- [x] Network registration and signal quality
- [x] Authorized user verification
- [x] Memory cleanup and privacy protection

### Network Performance

- **Signal Quality:** Excellent (+CSQ: 26,0)
- **Network Registration:** Successful (+CREG: 0,1)
- **Call Success Rate:** 100% for programmed numbers
- **SMS Delivery:** Reliable delivery and confirmation

## Development Priorities for v0.3.0

### Immediate Next Steps

1. **Audio System Integration**

   - Add microphone with proper preamp circuitry
   - Integrate dedicated speaker with power amplifier
   - Test complete audio path (mic → SIM900A → speaker)

2. **Hardware Organization**

   - Move to larger breadboard or design PCB layout
   - Integrate JZK XL4015 DC-DC converter as primary power solution
   - Add proper connectors for audio components
   - Optimize power distribution layout

3. **Extended Testing**
   - 1 week continuous operation test
   - Power consumption measurement and optimization
   - Thermal performance validation

### Medium-term Goals

1. **Audio Quality Enhancement**

   - Implement echo cancellation (CS48L32-CWZR evaluation)
   - Audio level control and automatic gain
   - Background noise suppression

2. **Complete Software Implementation**

   - Robust error handling and recovery
   - Complete call flow management
   - Enhanced SMS functionality

3. **Physical Design**
   - Determine optimal form factor based on component choices
   - Design accessible button layout
   - Create proper enclosure concept

## Current Stage Assessment

**We are at:** Proof of Concept - GSM Communication Validated  
**Progress:** ~20% toward functional prototype  
**Key Achievement:** Confirmed ESP32-WROOM-32 + SIM900A is viable platform  
**Next Milestone:** Complete audio integration and extended reliability testing

This prototype validates our core technology choices but significant development work remains before we have a functional accessibility device ready for real-world use.
