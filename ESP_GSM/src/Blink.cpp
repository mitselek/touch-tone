// Blink.cpp
#include <stdio.h>
#include <string.h> // For string functions like strstr, atoi
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "driver/ledc.h"
#include "driver/uart.h" // Add this include for uart_config_t
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_timer.h"
#include "private_config.h"
#include "constants.h"
#include "led_control.h"
#include "modem_control.h"
#include "settings.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char *TAG = "";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/

// Select the active pattern (can be changed at runtime)
// Make active_pattern volatile as it can be changed by another task/ISR (SMS handler)
volatile uint8_t active_pattern = PATTERN_BREATHING;

// Forward declarations
void process_command(char cmd);
// The modem function declarations are now in modem_control.h

int brightness = 0;

// Add at top, before process_command:
static char last_at_cmd[128] = {0};
// throttle ‘r’ repeats to 5 Hz
static uint32_t last_ms = 0;

char predefined_phone_number[32] = PREDEFINED_PHONE_NUMBER;

// Process input commands (for interactive mode)
void process_command(char cmd) {
    switch (cmd) {
        case 'h':
            ESP_LOGI(TAG, "Available commands:");
            ESP_LOGI(TAG, "  m: Check SIM800L modem status");
            ESP_LOGI(TAG, "  p: Enter PIN code for SIM card");
            ESP_LOGI(TAG, "  t: Send test SMS to predefined number");
            ESP_LOGI(TAG, "  h: Help - show this message");
            ESP_LOGI(TAG, "  s: Status - show current settings");
            ESP_LOGI(TAG, "  q: Quit - stop the program");
            ESP_LOGI(TAG, "  a: Send adhoc AT command");
            ESP_LOGI(TAG, "  r: Repeat last AT command");
            ESP_LOGI(TAG, "  n: Change predefined phone number for SMS test");
            break;
        case 's':
            ESP_LOGI(TAG, "Status:");
            ESP_LOGI(TAG, "  LED GPIO: %d", BLINK_GPIO);
            ESP_LOGI(TAG, "  Pattern: %d", active_pattern);
            ESP_LOGI(TAG, "  Period: %d ms", BLINK_PERIOD_MS);
            ESP_LOGI(TAG, "  Free heap: %" PRIu32 " bytes", esp_get_free_heap_size());
            break;
        case 'q':
            ESP_LOGW(TAG, "Stopping program execution!");
            vTaskDelete(NULL);
            break;
        case 'm':  // Check modem status
            ESP_LOGI(TAG, "Checking SIM800L modem status...");
            if (modem_check_sim_status() == ESP_OK) {
                print_network_status();
                print_signal_quality();
            }
            break;

        case 'p':  // Enter PIN code
            {
                ESP_LOGI(TAG, "Enter PIN code through serial (4 digits)");
                char pin[5] = {0};
                int chars_read = 0;
                
                // Use timeout in the while loop to avoid blocking indefinitely
                uint32_t start_time = esp_timer_get_time() / 1000;
                
                // Wait for 4 digits with timeout to prevent blocking
                while (chars_read < 4 && (esp_timer_get_time() / 1000 - start_time < 30000)) { // 30 sec timeout
                    uint8_t data;
                    int len = uart_read_bytes(UART_NUM_0, &data, 1, 500 / portTICK_PERIOD_MS); // Shorter timeout
                    
                    // Yield to other tasks periodically
                    vTaskDelay(1);
                    
                    if (len > 0 && data >= '0' && data <= '9') {
                        pin[chars_read++] = data;
                        uart_write_bytes(UART_NUM_0, &data, 1);  // Echo the character
                    }
                }
                
                if (chars_read == 4) {
                    ESP_LOGI(TAG, "Submitting PIN: %s", pin);
                    modem_enter_pin(pin);
                } else {
                    ESP_LOGW(TAG, "PIN entry timed out or was cancelled");
                }
            }
            break;
        case 't':  // 't' for "Text message"
            ESP_LOGI(TAG, "Sending test SMS to predefined number...");
            if (modem_check_sim_status() == ESP_OK) {
                esp_err_t sms_result = modem_send_sms(predefined_phone_number, "Hello World from ESP32!");
                if (sms_result == ESP_OK) {
                    ESP_LOGI(TAG, "Test SMS sent successfully!");
                } else {
                    ESP_LOGE(TAG, "Failed to send test SMS");
                }
            } else {
                ESP_LOGE(TAG, "SIM not ready, cannot send SMS");
            }
            break;
        case 'a':
            {
                ESP_LOGI(TAG, "Enter AT command (end with Enter):");
                char cmd_buf[128] = {0};
                int idx = 0;
                // Read until newline or buffer full
                while (idx < sizeof(cmd_buf) - 1) {
                    uint8_t ch;
                    int len = uart_read_bytes(UART_NUM_0, &ch, 1, portMAX_DELAY);
                    if (len > 0) {
                        if (ch == '\r' || ch == '\n') break;
                        cmd_buf[idx++] = ch;
                        uart_write_bytes(UART_NUM_0, &ch, 1);  // echo
                    }
                }
                cmd_buf[idx] = '\0';
                // save last
                strncpy(last_at_cmd, cmd_buf, sizeof(last_at_cmd)-1);
                last_at_cmd[sizeof(last_at_cmd)-1] = '\0';
                ESP_LOGI(TAG, "Sending AT→ %s", cmd_buf);
                char resp[MODEM_BUFFER_SIZE] = {0};
                esp_err_t r = modem_send_command(cmd_buf, resp, sizeof(resp), MODEM_COMMAND_TIMEOUT);
                if (r == ESP_OK) {
                    ESP_LOGI(TAG, "Response: %s", resp);
                } else {
                    ESP_LOGE(TAG, "AT error: %s", esp_err_to_name(r));
                }
            }
            break;
        case 'r':
            if (last_at_cmd[0]) {
                uint32_t now = esp_timer_get_time() / 1000;
                if (now - last_ms < 200) {
                    // slower than 5 Hz
                    break;
                }
                last_ms = now;
                ESP_LOGI(TAG, "Repeating AT→ %s", last_at_cmd);
                char resp[MODEM_BUFFER_SIZE] = {0};
                esp_err_t r = modem_send_command(last_at_cmd, resp, sizeof(resp), MODEM_COMMAND_TIMEOUT);
                if (r == ESP_OK) {
                    ESP_LOGI(TAG, "Response: %s", resp);
                } else {
                    ESP_LOGE(TAG, "AT error: %s", esp_err_to_name(r));
                }
            } else {
                ESP_LOGW(TAG, "No AT command to repeat");
            }
            break;
        case 'n':  // Change predefined phone number
            {
                ESP_LOGI(TAG, "Enter new predefined phone number (end with Enter):");
                char new_number[32] = {0};
                int idx = 0;
                // Read until newline or buffer full
                while (idx < sizeof(new_number) - 1) {
                    uint8_t ch;
                    int len = uart_read_bytes(UART_NUM_0, &ch, 1, portMAX_DELAY);
                    if (len > 0) {
                        if (ch == '\r' || ch == '\n') break;
                        new_number[idx++] = ch;
                        uart_write_bytes(UART_NUM_0, &ch, 1);  // echo
                    }
                }
                new_number[idx] = '\0';

                if (strlen(new_number) > 0) {
                    strncpy(predefined_phone_number, new_number, sizeof(predefined_phone_number) - 1);
                    predefined_phone_number[sizeof(predefined_phone_number) - 1] = '\0';
                    ESP_LOGI(TAG, "Predefined phone number updated to: %s", predefined_phone_number);
                } else {
                    ESP_LOGW(TAG, "No phone number entered, keeping the previous one.");
                }
            }
            break;
        default:
            break;
    }
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);  // Set log level to INFO globally

    ESP_LOGI(TAG, "Starting blink example on GPIO %d", BLINK_GPIO);
    ESP_LOGI(TAG, "Press 'h' for help with available commands");

    // Configure SIM800L modem
    configure_modem();

    // Initial modem status check
    ESP_LOGI(TAG, "Performing initial modem check...");
    modem_check_sim_status();

    // Initialize the modem (Keep if using SIM800L)
    // if (modem_init() != ESP_OK) {
    //     ESP_LOGE(TAG, "Modem initialization failed! SMS control disabled.");
    // } else {
    //      ESP_LOGI(TAG, "Modem ready for SMS control.");
    // }

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Load settings from NVS
    load_settings();

    // Configure LED PWM using individual assignments
    ledc_timer_config_t ledc_timer;
    memset(&ledc_timer, 0, sizeof(ledc_timer));
    ledc_timer.speed_mode = LEDC_SPEED_MODE;
    ledc_timer.duty_resolution = LEDC_DUTY_RESOLUTION;
    ledc_timer.timer_num = LEDC_TIMER_NUM;
    ledc_timer.freq_hz = LEDC_PWM_FREQUENCY;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    // Configure LED controller completely manually to avoid any ordering issues
    ledc_channel_config_t ledc_channel = {
        .gpio_num = BLINK_GPIO,
        .speed_mode = LEDC_SPEED_MODE,
        .channel = LEDC_CHANNEL_NUM,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_NUM,
        .duty = 0,
        .hpoint = 0,
        .sleep_mode = (ledc_sleep_mode_t)0,  // Cast to the correct enum type
        .flags = {
            .output_invert = 0
        }
    };
    
    // Apply the channel configuration
    esp_err_t err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error configuring LED channel: %s", esp_err_to_name(err));
    }

    // Install UART driver for console input on UART0
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);

    while (1) {
        // Check for input from serial console (for local control)
        uint8_t input_buffer[64];
        int length = uart_read_bytes(UART_NUM_0, input_buffer, sizeof(input_buffer), 0);  // Non-blocking
        if (length > 0) {
            for (int i = 0; i < length; i++) {
                process_command(input_buffer[i]);
            }
        }

        // Execute the selected LED pattern
        execute_pattern();

        // Save settings to NVS periodically
        static int save_counter = 0;
        if (++save_counter >= 1e4) {
            save_settings();
            save_counter = 0;
            // Yield to other tasks right after NVS operations which can be intensive
            vTaskDelay(1);
        }

        // Always add a small delay to prevent this loop from hogging CPU
        // This ensures the watchdog gets fed regularly
        vTaskDelay(1);  // Just 1 tick is enough to yield the CPU and avoid WDT issues
    }
}

#ifdef __cplusplus
}
#endif
