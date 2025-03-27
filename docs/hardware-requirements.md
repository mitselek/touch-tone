# Hardware requirements for desktop phone

## Executive Summary

This report identifies the essential hardware components required for a custom bedside phone designed for an elderly blind user. The device aims to replace traditional visual interfaces with tactile inputs and non-visual feedback mechanisms for enhanced accessibility.

The recommended battery configuration includes a 3.7V LiPo with integrated protection circuits (BMS), compatible with the ESP32's operating voltage. A high-quality speaker optimized for voice communication is essential, with emphasis on clarity rather than size constraints.

For core processing and connectivity, the report suggests using either an ESP32 or Arduino microcontroller paired with a 4G module, with integrated boards like the Waveshare ESP32-S3 SIM7670G or Makerfabs ESP32S3 4G LTE CAT1 A7670X identified as strong candidates due to their combined processing and cellular capabilities.

Additional required components include buttons with distinct tactile features for call control and answering, power management systems to handle mains power and battery backup, and tactile feedback via a vibration module. The device is designed as an always-on, stationary unit with intelligent power management to maximize battery life during power outages.

## Battery
- Integrated protection circuits (BMS or PCM)
- 3.7V aligns well with the operating voltage of ESP32 and can also power Arduino boards via USB
- Built-in BMS is strongly recommended to ensure essential protection features are in place without the need for a separate module

## Speaker
- Top-quality speaker that can deliver clear audio for hands-free calls
- Specifically designed for voice communication
- To ensure long battery life, the speaker should also have low power consumption
- Size is not be a factor as device is not hand-held.
- A key consideration is the speaker's impedance in relation to the chosen microcontroller's audio output capabilities.

## Core logic and communication
The core of the phone will be built around either an ESP32 or an Arduino microcontroller, providing the processing power for managing the phone's functions. Cellular network access for making calls and sending SMS battery status updates will be facilitated by a 4G module. The microcontroller will also need to store and associate pre-programmed phone numbers with each of the tactile buttons.

For this project, an integrated ESP32 board with either the SIM7670G or A7670 4G module appears to be a strong contender due to its focus on low power consumption and the direct combination of processing and cellular connectivity.
- Waveshare ESP32-S3 SIM7670G 4G Development Board combines an ESP32-S3 with a SIM7670G 4G Cat-1 module. This board supports various features including WiFi, Bluetooth, GNSS, and offers low power working modes, making it suitable for battery-operated devices. 
  The SIM7670G module supports LTE Cat-1, which provides sufficient data rates (up to 10Mbps downlink and 5Mbps uplink) for voice calls and SMS.
- Makerfabs ESP32S3 4G LTE CAT1 A7670X integrates an ESP32-S3 with an A7670 Cat-1 4G module, also featuring WiFi, Bluetooth, GNSS, a USB-C connector, and onboard charging capabilities.
  The A7670 module also supports LTE Cat-1.
  
These integrated boards typically operate at 3.3V, which is compatible with a 3.7V LiPo battery (with a small voltage drop) or can be powered via USB at 5V.

## Buttons and Input System
The phone will feature large tactile buttons (4-8) with distinct shapes and textures to assist blind users in identifying different functions. These buttons will be used for:
- Making calls to pre-programmed numbers
- Answering incoming calls (any button can answer a call)
- Ending calls
- Potentially adjusting volume or accessing other basic functions

Requirements for the buttons:
- Large surface area for easy location and pressing
- Distinct tactile features for differentiation
- Reliable mechanical design for frequent use
- Clear tactile feedback when pressed

## Movement Sensor
To enable call answering by simply moving or tumbling on the device.
To detect, if device is dropped from table.
Combination of gyroscope, accelerometer as well a brobrospectrum magnetic field measurements should provide reliable readings.

## Power Management and Charging
The phone is meant to be always connected to mains. Large battery is included to make sure it can survive extensive no-power periods. Device should manage battery charge smartly. The design will be always-on, with no physical power switch. 
The microcontroller will manage a low-power mode and send battery status updates via SMS, when mains power is absent. As a single-piece stationary device, it connects directly to mains power through an integrated power adapter, with the USB-C port serving as a backup power option.

## Tactile Feedback - Vibration Module
A hardware-level vibration module is needed to provide tactile alerts for button press and potentially for other feedback, such as call answering confirmation.

Both cylindrical and coin types should be considered based on the phone's design. Motors with an operating voltage compatible with the chosen battery (e.g., 3V for a 3.7V battery)

