#pragma once
#include <stdint.h>
#include "driver/gpio.h"
#include "constants.h"  // Include the new constants header

#ifdef __cplusplus
extern "C" {
#endif

void configure_led(void);
void led_on(void);
void led_off(void);
void set_brightness(uint8_t brightness_percent);
void execute_pattern(void);

#ifdef __cplusplus
}
#endif