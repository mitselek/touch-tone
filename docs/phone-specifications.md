# Custom Bedside Phone Specifications for Elderly Blind User

## Executive Summary

This document outlines the specifications for a custom bedside phone designed specifically for an elderly blind user. The phone features large tactile buttons instead of a screen, with a customizable layout of 4-8 buttons arranged in two columns. The device supports customizable tactile button caps that can be arranged as pairs or as single larger buttons covering an entire row.

The phone integrates a substantial LiPo battery for extended off-grid operation, high-quality speaker for clear calls, and uses either an ESP32 or Arduino microcontroller with a 4G module for cellular connectivity. Key accessibility features include movement-based call answering, battery status notifications via SMS, and hardware-level vibration feedback.

The device is designed for continuous operation as a stationary, mains-powered unit with a USB-C port for backup power. Power management includes an always-on design with low-power optimization and comprehensive battery monitoring. The phone provides audio and vibration feedback for important operations and status changes, enhancing usability for the visually impaired user.

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
*   **Button-Based Call Answering:** Any button press will answer an incoming call.
*   **Direct Mains Connection:** Single-piece stationary device connected directly to mains power.
*   **USB-C Port:** Alternative charging option for flexibility and backup during power outages.
*   **Vibration Module:** Hardware-level vibration capability for alerts and future software enhancements.

## Accessibility Features

*   **No Visual Interface:** Designed specifically for blind users.
*   **Large Tactile Buttons:** Easy to locate and press.
*   **Button-Based Call Answering:** Simplifies answering calls by allowing any button to accept an incoming call.
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

