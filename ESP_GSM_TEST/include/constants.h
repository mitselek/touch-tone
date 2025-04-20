#pragma once

#include "driver/gpio.h"

// LED pattern constants
#define PATTERN_SOLID 0
#define PATTERN_BREATHING 1
#define PATTERN_BLINKING 2
#define PATTERN_PULSE 3
#define PATTERN_MANUAL_BRIGHTNESS 4

// LED pattern parameters
#define BREATHING_STEP 1
#define BREATHING_MAX 255
#define BLINK_ON_TIME_MS 300
#define BLINK_OFF_TIME_MS 200
#define PULSE_ON_TIME_MS 100
#define PULSE_PAUSE_MS 1000

// Brightness control parameters
#define MANUAL_BRIGHTNESS_MAX 100
#define MANUAL_BRIGHTNESS_STEP 10
#define TASK_DELAY_BRIGHTNESS_THRESHOLD 30

// PWM parameters
#define LEDC_TIMER_NUM LEDC_TIMER_0
#define LEDC_SPEED_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_NUM LEDC_CHANNEL_0
#define LEDC_DUTY_RESOLUTION LEDC_TIMER_8_BIT  // 8-bit resolution for duty cycle (0-255)
#define LEDC_PWM_FREQUENCY 5000              // 5kHz PWM frequency

// Modem configuration parameters
#define MODEM_UART_NUM        UART_NUM_2  // Using UART2 for SIM800L
#define MODEM_TX_PIN          4           // ESP32 TX pin connected to SIM800L RX
#define MODEM_RX_PIN          0           // ESP32 RX pin connected to SIM800L TX
#define MODEM_BUFFER_SIZE     1024        // Size of buffer for reading from modem
#define MODEM_COMMAND_TIMEOUT 2000        // Maximum wait time in ms for most AT commands
#define MODEM_CPIN_TIMEOUT    5000        // Extended timeout for AT+CPIN? (ms)

// LED blink example defaults
#define BLINK_GPIO_NUM   5      // GPIO used for the blink example
#define BLINK_PERIOD_MS 1000    // default blink period
#define BLINK_GPIO      ((gpio_num_t)BLINK_GPIO_NUM)