# Custom Bedside Phone Specifications for Elderly Blind User

## Core Features

*   **No Screen:** The device will have no visual display.
*   **Tactile Buttons:**
    *   Fixed number of very large tactile buttons (customizable from 4 to 8).
    *   Button arrangement: 2 columns with up to 4 buttons per column.
    *   Flexible button caps:
        *   Option to attach 2 separate button caps with distinct tactile features to each row.
        *   Option to attach one larger button cap covering both buttons in a row (e.g., a large emergency button).
*   **Massive Battery:** Big LiPo battery for potentially months of off-grid operation.
*   **High-Quality Speaker:** Top-quality speaker for clear hands-free calls.
*   **Microcontroller:** Utilizes either ESP32 or Arduino.
*   **Connectivity:** 4G module for cellular network access.
*   **Pre-programmed Numbers:** Hard-coded phone numbers assigned to each button.
*   **Movement Sensor:** Sensor to detect phone movement for answering calls by simply moving or tapping the phone.
*   **Direct Mains Connection:** Single-piece stationary device connected directly to mains power.
*   **USB-C Port:** Alternative charging option for flexibility and backup during power outages.
*   **Vibration Module:** Hardware-level vibration capability for alerts and future software enhancements.

## Accessibility Features

*   **No Visual Interface:** Designed specifically for blind users.
*   **Large Tactile Buttons:** Easy to locate and press.
*   **Movement-Based Call Answering:** Simplifies answering calls.
*   **Battery Status via SMS:** Comprehensive battery level notifications:
    *   SMS sent at 100%, 75%, 50%, 25%, 10%, and 5% battery levels.
*   **Low-Battery Alert:** Audible sound and vibration when battery reaches critical levels.
*   **Reassuring Sound:** Produces a sound when confirming important operations or status changes.

## Power Management

*   **Always-On Design:** Device configured for continuous operation over extended periods.
*   **Low-Power Mode:** Microcontroller optimized for minimal power consumption during idle states.
*   Battery status updates sent via SMS at specific charge levels.
*   USB-C port provides alternative charging method to direct mains connection.
*   **Power Configuration:** 
    *   No physical power switch
    *   Device designed to remain operational for years without manual power cycling
    *   Power management handled through software and hardware-level power optimization techniques
*   **Mains Power Connection:** Single-piece stationary device with direct mains power connection via integrated power adapter.

## Feedback Mechanisms

*   Audible confirmation sounds for successful operations.
*   Audible and vibration alerts for battery status.
*   No voice feedback implemented initially (potential future feature).

## Hardware Specifications Update

*   **Battery Monitoring:** Enhanced battery tracking with multiple notification points.
*   **Alert System:** 
    *   Vibration module for tactile alerts.
    *   Speaker-based audible alerts.
*   **Power Options:**
    *   Direct mains connection (primary method)
    *   USB-C port (backup power during outages)

