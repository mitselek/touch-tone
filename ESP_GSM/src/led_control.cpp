#include "led_control.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"  // For esp_timer_get_time()
#include "constants.h"  // Include the new constants file

static const char *TAG = "LED Control";

// These variables are defined in Blink.cpp, but we need to declare them here with extern
// Removed unused pattern control variables
// extern volatile uint8_t active_pattern;
// extern int brightness;

static uint8_t s_led_state = 0;

void configure_led(void) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_SPEED_MODE,
        .duty_resolution = LEDC_DUTY_RESOLUTION,
        .timer_num = LEDC_TIMER_NUM,
        .freq_hz = LEDC_PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = 0  // Explicitly initialize missing member
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = BLINK_GPIO,
        .speed_mode = LEDC_SPEED_MODE,
        .channel = LEDC_CHANNEL_NUM,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_NUM,
        .duty = 0,
        .hpoint = 0,
        .flags = {
            .output_invert = 0  // Explicitly initialize missing member
        }
    };
    ledc_channel_config(&ledc_channel);
}

void led_on(void) {
    s_led_state = 1;
    gpio_set_level(BLINK_GPIO, s_led_state);
    ESP_LOGD(TAG, "LED ON");
}

void led_off(void) {
    s_led_state = 0;
    gpio_set_level(BLINK_GPIO, s_led_state);
    ESP_LOGD(TAG, "LED OFF");
}

void set_brightness(uint8_t brightness_percent) {
    uint32_t duty = (brightness_percent * 255) / 100;  // Convert percentage to duty cycle value
    ESP_LOGD(TAG, "Setting LED brightness to %d%% (duty: %lu)", brightness_percent, duty);
    ledc_set_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_NUM, duty);
    ledc_update_duty(LEDC_SPEED_MODE, LEDC_CHANNEL_NUM);
}

void execute_pattern(void) {
    static uint8_t breathing_value = 0;
    static bool breathing_direction = true;
    uint32_t delay_ms;

    // Breathing effect: fade in and out
    if (breathing_direction) {
        breathing_value += BREATHING_STEP;
        if (breathing_value >= BREATHING_MAX) {
            breathing_direction = false;
        }
    } else {
        if (breathing_value <= BREATHING_STEP) {
            breathing_value = 0;
            breathing_direction = true;
            vTaskDelay(1);  // yield
        } else {
            breathing_value -= BREATHING_STEP;
        }
    }

    // Update brightness
    set_brightness((breathing_value * 100) / BREATHING_MAX);
    delay_ms = 5 + (BREATHING_MAX - breathing_value) / 10;
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
}