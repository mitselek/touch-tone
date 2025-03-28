# Phone Zero MVP Requirements

## Executive Summary

This document defines the minimum viable product (MVP) requirements for Phone Zero, reducing scope to essential features needed to validate the core concept of a bedside phone for elderly blind users. The MVP focuses exclusively on critical functionality: making and receiving calls via large tactile buttons, providing basic audio feedback, and ensuring reliable power operation with minimal battery backup capability.

By limiting the scope to these essential features, development time can be significantly reduced from months to weeks, allowing for faster prototyping, testing, and iteration based on user feedback. This approach prioritizes functional validation over feature completeness, with the understanding that additional capabilities can be incorporated in subsequent iterations.

---

## Core Functionality Requirements

### 1. Basic Calling Capabilities

- **Make calls:** Ability to call 6 pre-programmed phone numbers using dedicated buttons
- **Receive calls:** Basic ringtone with ability to answer by pressing any button
- **End calls:** End active calls by pressing any button
- **Audio quality:** Sufficient clarity for intelligible voice communication

### 2. Minimum Physical Interface

- **Button count:** 6 total buttons (4 regular, 2 double)
- **Button size:** Large enough for easy location and operation by elderly users
- **Button differentiation:** Basic tactile differences between buttons (shape/texture/size)
- **Audio feedback:** Simple sounds to confirm button presses and operations
- **Speaker:** Basic speaker with adequate volume for voice calls

### 3. Essential Hardware

- **Microcontroller:** ESP32 (using development board for simplicity)
- **Connectivity:** Basic 4G module (SIM7670G or A7670) with voice capability
- **Audio components:** Microphone and speaker with minimal amplification
- **Power input:** USB-C connection for power and charging
- **Battery:** Small LiPo for demo purposes (2-8 hour backup vs. months in full version)
- **Enclosure:** Simple protective case (3D printed or modified existing box)

### 4. Minimum Power Management

- **Power modes:** Basic on/off functionality
- **Power switching:** Simple circuit to switch between USB-C power and battery
- **Battery monitoring:** Basic voltage monitoring without complex BMS features
- **Charging:** USB-C compatible charging circuit (leveraging standard USB PD where possible)

---

## Features Explicitly Deferred for Post-MVP

1. **Extended battery life** (months of backup operation)
2. **Advanced tactile button designs** (custom molded shapes, multiple textures)
3. **SMS battery notifications**
4. **Movement sensor** for call answering
5. **Vibration feedback**
6. **Advanced battery management** (adaptive charging, temperature compensation)
7. **Extensive power optimization** for ultra-low consumption
8. **Enhanced audio processing** (noise reduction, echo cancellation)
9. **Premium speaker and audio components**
10. **Sophisticated enclosure design** with professional finish
11. **Firmware update capabilities**
12. **Self-diagnostic routines**
13. **Direct mains power connection** (using standard power adapter instead of USB-C)

## Success Criteria for MVP

The MVP will be considered successful if it demonstrates:

1. **Functional calling:** Can reliably make and receive calls
2. **Blind usability:** Can be operated without visual cues
3. **Operational stability:** Functions consistently without crashes or resets
4. **Power resilience:** Operates on mains power and switches to battery when needed
5. **Basic feedback:** Provides adequate audio confirmation of actions

## Development Approach

The MVP development will focus on:

1. **Using off-the-shelf components** rather than custom designs where possible
2. **Leveraging development boards** instead of custom PCBs
3. **Simplified firmware** covering only essential functionality
4. **Rapid prototyping** using breadboards and standard connectors
5. **Manual assembly** techniques that don't require specialized equipment

This approach prioritizes speed of implementation and functional validation over product refinement, allowing for basic user testing and concept validation within weeks rather than months.
