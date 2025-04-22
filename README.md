# Phone Zero

## Executive Summary

Phone Zero is a custom bedside telephone designed specifically for elderly blind users, prioritizing accessibility through tactile interfaces rather than visual displays. The device features large, distinctly textured buttons, button-based call answering functionality, and non-visual feedback mechanisms to create an intuitive user experience.

The phone operates as an always-on stationary device with continuous mains power connection and battery backup for uninterrupted operation during outages. It incorporates high-quality audio components optimized for voice clarity, and a comprehensive monitoring system that sends battery status updates via SMS.

This project documentation covers hardware requirements, phone specifications, power management strategies, audio processing considerations, and component selection guidelines. The development roadmap includes creating detailed schematics, building an initial prototype, designing an ergonomic physical enclosure, developing firmware, sourcing components, creating specialized tactile button caps, implementing power management systems, and producing 3D models for the enclosure.

Phone Zero aims to provide a reliable, accessible communication device that can operate for extended periods without technical intervention, making it ideal for elderly users with visual impairments who need a dependable and simple way to maintain contact with family and caregivers.

---

## Project Overview

Phone Zero is a specialized communication device that eliminates traditional visual interfaces in favor of tactile buttons, button-based call answering, and non-visual feedback mechanisms. This stationary device is designed to be reliable, easy to use, and capable of long-term operation without technical intervention.

## Key Features

- **No Screen:** Designed specifically for blind users
- **Large Tactile Buttons:** 4-8 customizable buttons with distinct tactile features
- **Button-Based Answering:** Answer calls by pressing any button
- **Always-On Design:** Continuous operation with battery backup for power outages
- **Battery Status via SMS:** Automatic notifications at specific battery levels
- **High-Quality Audio:** Optimized for clear voice communication
- **Long Battery Life:** Extended off-grid operation capability

## Current Documentation

- [Hardware Requirements](docs/hardware-requirements.md) - Detailed specifications for core components
- [Phone Specifications](docs/phone-specifications.md) - Core features and accessibility design
- [Always-On Design Strategies](docs/always-on-design-strategies.md) - Power management and longevity approaches
- [Audio Signal Flow](docs/audio-signal-flow.md) - Audio pathway from microphone to speaker
- [Audio Codec Chip Selection](docs/audio-codec-chip-selection.md) - Analysis of suitable audio codecs
- [Battery Management](docs/battery-management.md) - Battery protection and charging optimization

## Next Development Steps

### 1. [Create Detailed Schematic Diagram](https://github.com/mitselek/touch-tone/issues/1)

- Design the complete electronic circuitry with proper voltage levels and signal pathways
- Define connections between ESP32/microcontroller, 4G module, speaker, sensors, and power system
- Include test points and safety mechanisms

### 2. [Develop Initial Prototype](https://github.com/mitselek/touch-tone/issues/2)

- Breadboard the core components to test basic functionality
- Verify integration between microcontroller, 4G module, and sensors
- Test power requirements and communication protocols

### 3. [Design Physical Enclosure](https://github.com/mitselek/touch-tone/issues/3)

- Focus on ergonomics for elderly users
- Incorporate large tactile buttons with distinct shapes and textures
- Ensure clear speaker positioning and easy access to USB-C port

### 4. [Develop Firmware](https://github.com/mitselek/touch-tone/issues/4)

- Implement button handling, call management, and SMS notifications
- Create button-based call answering functionality
- Optimize power management for battery longevity

### 5. [Source Hardware Components](https://github.com/mitselek/touch-tone/issues/5)

- Research and select reliable suppliers for all components
- Obtain samples for testing before final selection
- Consider customization options for tactile elements

### 6. [Design Tactile Button Caps](https://github.com/mitselek/touch-tone/issues/6)

- Explore various shapes, textures, and materials
- Test individual caps and larger unified caps
- Optimize for touch recognition and ease of use

### 7. [Develop Power Management System](https://github.com/mitselek/touch-tone/issues/7)

- Create seamless transition between mains power and battery
- Implement adaptive charging and battery health monitoring
- Design low-power modes for extended operation

### 8. [Create 3D Models for Enclosure](https://github.com/mitselek/touch-tone/issues/8)

- Model internal component mounting and external features
- 3D print prototypes for testing fit and ergonomics
- Iterate based on physical testing results

## Setup Instructions

### Private Configuration

This project uses a private configuration file for sensitive information like phone numbers.

To set up:
1. Navigate to `ESP_GSM/include/`
2. Edit `private_config.h` and replace the placeholder values with your actual information

The `private_config.h` file is ignored by Git to prevent accidentally committing sensitive information.

## Contributing

This project is currently in development. If you're interested in contributing, please contact the project maintainers.

## License

[License information to be added]
