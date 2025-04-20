#include "modem_control.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_timer.h" // For esp_timer_get_time()
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "private_config.h"
#include "constants.h"  // Include centralized constants

static const char *TAG = "Modem Control";

// AT command responses
#define AT_OK      "OK"
#define AT_ERROR   "ERROR"
#define AT_CME     "+CME ERROR"
#define AT_CMS     "+CMS ERROR"
#define AT_TIMEOUT "TIMEOUT"

// Static buffer for modem responses
static char modem_buffer[MODEM_BUFFER_SIZE];

void configure_uart_modem(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_APB,
        .flags = 0
    };
    uart_driver_install(MODEM_UART_NUM, MODEM_BUFFER_SIZE * 2, MODEM_BUFFER_SIZE * 2, 0, NULL, 0);
    uart_param_config(MODEM_UART_NUM, &uart_config);
    uart_set_pin(MODEM_UART_NUM, MODEM_TX_PIN, MODEM_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

/**
 * @brief Configure UART for communicating with the modem
 */
void configure_modem(void) {
    ESP_LOGI(TAG, "Configuring modem on UART%d (TX: GPIO%d, RX: GPIO%d)...", 
             MODEM_UART_NUM, MODEM_TX_PIN, MODEM_RX_PIN);
    
    uart_driver_delete(MODEM_UART_NUM);
    configure_uart_modem();
    
    uart_flush(MODEM_UART_NUM);

    ESP_LOGI(TAG, "Waiting for modem to stabilize (5 seconds)...");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    
    ESP_LOGI(TAG, "Attempting modem reset...");
    char response[128] = {0};
    bool reset_success = false;
    for (int i = 0; i < 3; i++) {
        if (modem_send_command("AT+CFUN=1,1", response, sizeof(response), 5000) == ESP_OK) {
            reset_success = true;
            ESP_LOGI(TAG, "Modem reset successful");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            break;
        }
        ESP_LOGW(TAG, "Modem reset command failed (attempt %d/3)", i + 1);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    if (!reset_success) {
        ESP_LOGE(TAG, "Modem reset failed after multiple attempts");
    }
    
    bool modem_responding = false;
    for (int i = 0; i < 5; i++) {
        ESP_LOGI(TAG, "Sending AT command (attempt %d/5)...", i+1);
        if (modem_send_command("AT", response, sizeof(response), 3000) == ESP_OK) {
            ESP_LOGI(TAG, "Modem responded to AT command!");
            modem_responding = true;
            break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    if (modem_responding) {
        if (modem_send_command("ATI", response, sizeof(response), 3000) == ESP_OK) {
            ESP_LOGI(TAG, "Modem info: %s", response);
        }
        
        if (modem_send_command("AT+CMGF=1", response, sizeof(response), 3000) == ESP_OK) {
            ESP_LOGI(TAG, "SMS text mode enabled");
        }
        
        modem_send_command("AT+CNMI=1,0,0,0,0", response, sizeof(response), 3000);
        modem_send_command("AT+CPMS=\"SM\",\"SM\",\"SM\"", response, sizeof(response), 3000);
        
        ESP_LOGI(TAG, "Checking SIM status...");
        esp_err_t sim_status = ESP_FAIL;
        for (int i = 0; i < 3; i++) {
            sim_status = modem_check_sim_status();
            if (sim_status == ESP_OK) {
                ESP_LOGI(TAG, "SIM is ready!");
                break;
            } else if (sim_status == ESP_ERR_NOT_FINISHED) {
                ESP_LOGW(TAG, "SIM card requires PIN code");
                break;
            } else {
                ESP_LOGW(TAG, "SIM status check failed (attempt %d/3)", i + 1);
                vTaskDelay(2000 / portTICK_PERIOD_MS);
            }
        }
        if (sim_status != ESP_OK) {
            ESP_LOGE(TAG, "Failed to verify SIM status");
        }
    } else {
        ESP_LOGE(TAG, "Modem not responding. Check hardware connections.");
    }
}

/**
 * @brief Send AT command to the modem and wait for response
 * 
 * @param command AT command to send
 * @param response Buffer to store response
 * @param response_size Size of response buffer
 * @param timeout_ms Timeout in milliseconds
 * @return esp_err_t ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t modem_send_command(const char* command, char* response, size_t response_size, uint32_t timeout_ms) {
    if (command == NULL || response == NULL || response_size == 0) {
        ESP_LOGE(TAG, "Invalid parameters for modem_send_command");
        return ESP_ERR_INVALID_ARG;
    }
    
    memset(response, 0, response_size);
    memset(modem_buffer, 0, MODEM_BUFFER_SIZE);
    uart_flush(MODEM_UART_NUM);
    
    char cmd_buf[256];
    snprintf(cmd_buf, sizeof(cmd_buf), "%s\r\n", command);
    ESP_LOGD(TAG, "Sending command: %s", command);
    uart_write_bytes(MODEM_UART_NUM, cmd_buf, strlen(cmd_buf));
    
    uint32_t start_time = esp_timer_get_time() / 1000;
    bool timeout_occurred = false;
    size_t total_bytes_read = 0;
    
    while ((esp_timer_get_time() / 1000 - start_time) < timeout_ms) {
        uint8_t data;
        int len = uart_read_bytes(MODEM_UART_NUM, &data, 1, 100 / portTICK_PERIOD_MS);
        
        if (len > 0) {
            if (total_bytes_read < MODEM_BUFFER_SIZE - 1) {
                modem_buffer[total_bytes_read++] = data;
                modem_buffer[total_bytes_read] = '\0';
            }
            
            if (strstr(modem_buffer, AT_OK) || 
                strstr(modem_buffer, AT_ERROR) || 
                strstr(modem_buffer, AT_CME) || 
                strstr(modem_buffer, AT_CMS)) {
                vTaskDelay(50 / portTICK_PERIOD_MS); // Yield to prevent WDT issues
                uart_read_bytes(MODEM_UART_NUM, (uint8_t*)modem_buffer + total_bytes_read, 
                                MODEM_BUFFER_SIZE - total_bytes_read - 1, 10 / portTICK_PERIOD_MS);
                break;
            }
        }
        
        vTaskDelay(1); // Yield to prevent WDT issues
    }
    
    if ((esp_timer_get_time() / 1000 - start_time) >= timeout_ms) {
        ESP_LOGW(TAG, "Command timed out: %s", command);
        timeout_occurred = true;
    }
    
    size_t copy_size = total_bytes_read < response_size - 1 ? total_bytes_read : response_size - 1;
    memcpy(response, modem_buffer, copy_size);
    response[copy_size] = '\0';
    
    char* cleaned_response = response;
    char* cleaned_ptr = cleaned_response;
    char* src_ptr = response;
    
    while (*src_ptr) {
        if (src_ptr == response && strncmp(src_ptr, command, strlen(command)) == 0) {
            src_ptr += strlen(command);
            while (*src_ptr == '\r' || *src_ptr == '\n') {
                src_ptr++;
            }
            continue;
        }
        
        if (cleaned_ptr == cleaned_response && (*src_ptr == '\r' || *src_ptr == '\n')) {
            src_ptr++;
            continue;
        }
        
        if (*src_ptr == '\r' || *src_ptr == '\n') {
            *cleaned_ptr++ = ' ';
            src_ptr++;
            while (*src_ptr == '\r' || *src_ptr == '\n') {
                src_ptr++;
            }
            continue;
        }
        
        *cleaned_ptr++ = *src_ptr++;
    }
    *cleaned_ptr = '\0';
    
    cleaned_ptr = cleaned_response + strlen(cleaned_response) - 1;
    while (cleaned_ptr >= cleaned_response && *cleaned_ptr == ' ') {
        *cleaned_ptr-- = '\0';
    }
    
    ESP_LOGD(TAG, "Response (%zu bytes): %s", strlen(cleaned_response), cleaned_response);
    
    if (strstr(cleaned_response, AT_OK)) {
        return ESP_OK;
    } else if (timeout_occurred) {
        return ESP_ERR_TIMEOUT;
    } else {
        return ESP_FAIL;
    }
}

/**
 * @brief Check SIM card status
 * 
 * @return esp_err_t ESP_OK if SIM is ready, ESP_FAIL otherwise
 */
esp_err_t modem_check_sim_status(void) {
    char response[128] = {0};
    
    if (modem_send_command("AT+CPIN?", response, sizeof(response), MODEM_COMMAND_TIMEOUT) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to check SIM status");
        return ESP_FAIL;
    }
    
    if (strstr(response, "+CPIN: READY")) {
        ESP_LOGI(TAG, "SIM card is ready");
        return ESP_OK;
    } else if (strstr(response, "+CPIN: SIM PIN")) {
        ESP_LOGW(TAG, "SIM card requires PIN");
        return ESP_ERR_NOT_FINISHED;
    } else if (strstr(response, "+CPIN: SIM PUK")) {
        ESP_LOGE(TAG, "SIM card requires PUK (blocked)");
        return ESP_ERR_NOT_SUPPORTED;
    } else {
        ESP_LOGE(TAG, "Unknown SIM status: %s", response);
        return ESP_FAIL;
    }
}

/**
 * @brief Enter PIN code for SIM card with improved error handling
 * 
 * @param pin_code PIN code as string
 * @return esp_err_t ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t modem_enter_pin(const char* pin_code) {
    if (pin_code == NULL || strlen(pin_code) < 4) {
        ESP_LOGE(TAG, "Invalid PIN code");
        return ESP_ERR_INVALID_ARG;
    }
    
    char cmd[32] = {0};
    char response[128] = {0};
    
    if (modem_send_command("AT+CPIN?", response, sizeof(response), MODEM_COMMAND_TIMEOUT) == ESP_OK) {
        if (strstr(response, "+CPIN: READY")) {
            ESP_LOGI(TAG, "SIM card is already unlocked, no PIN needed");
            return ESP_OK;
        } else if (!strstr(response, "+CPIN: SIM PIN")) {
            ESP_LOGW(TAG, "SIM status not asking for PIN: %s", response);
            if (strstr(response, "+CPIN: SIM PUK")) {
                ESP_LOGE(TAG, "SIM requires PUK code, not PIN - SIM is blocked");
                return ESP_ERR_INVALID_STATE;
            }
        }
    }
    
    ESP_LOGI(TAG, "Resetting modem before entering PIN...");
    modem_send_command("AT+CFUN=1,1", response, sizeof(response), 3000);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    
    snprintf(cmd, sizeof(cmd), "AT+CPIN=%s", pin_code);
    
    bool pin_accepted = false;
    int pin_attempts = 0;
    const int MAX_PIN_ATTEMPTS = 3;
    
    while (!pin_accepted && pin_attempts < MAX_PIN_ATTEMPTS) {
        pin_attempts++;
        ESP_LOGI(TAG, "Sending PIN to modem (attempt %d/%d)...", pin_attempts, MAX_PIN_ATTEMPTS);
        
        esp_err_t result = modem_send_command(cmd, response, sizeof(response), MODEM_COMMAND_TIMEOUT);
        
        if (result == ESP_OK) {
            ESP_LOGI(TAG, "PIN command accepted");
            pin_accepted = true;
        } else {
            ESP_LOGW(TAG, "PIN command failed: %s", response);
            
            if (result == ESP_ERR_TIMEOUT) {
                ESP_LOGI(TAG, "Resetting UART connection...");
                uart_flush(MODEM_UART_NUM);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            } else {
                vTaskDelay(2000 / portTICK_PERIOD_MS);
            }
        }
    }
    
    if (!pin_accepted) {
        ESP_LOGE(TAG, "Failed to enter PIN after multiple attempts");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "PIN accepted. Waiting for SIM to initialize (this may take several seconds)...");
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    
    int verify_retries = 10;
    bool pin_ok = false;
    
    while (verify_retries-- > 0 && !pin_ok) {
        ESP_LOGI(TAG, "Checking SIM status (%d tries left)...", verify_retries+1);
        
        if (modem_send_command("AT+CPIN?", response, sizeof(response), MODEM_COMMAND_TIMEOUT * 2) == ESP_OK) {
            if (strstr(response, "+CPIN: READY")) {
                ESP_LOGI(TAG, "SIM card is now ready");
                pin_ok = true;
                
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                
                if (modem_send_command("AT+CSQ", response, sizeof(response), MODEM_COMMAND_TIMEOUT) == ESP_OK) {
                    ESP_LOGI(TAG, "Signal quality: %s", response);
                }
                
                if (modem_send_command("AT+CREG?", response, sizeof(response), MODEM_COMMAND_TIMEOUT) == ESP_OK) {
                    ESP_LOGI(TAG, "Network registration status: %s", response);
                    
                    if (strstr(response, "+CREG: 0,1") || strstr(response, "+CREG: 0,5")) {
                        ESP_LOGI(TAG, "Network registered successfully");
                    } else {
                        ESP_LOGW(TAG, "Network not registered yet, but SIM is ready");
                    }
                }
                
                return ESP_OK;
            } else if (strstr(response, "+CPIN: SIM PIN")) {
                ESP_LOGW(TAG, "PIN was sent but SIM still asking for PIN (retry %d)", 10 - verify_retries);
            } else {
                ESP_LOGW(TAG, "Unknown SIM status after PIN entry: %s", response);
            }
        } else {
            ESP_LOGW(TAG, "Failed to check PIN status (retry %d)", 10 - verify_retries);
        }
        
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
    
    if (!pin_ok) {
        ESP_LOGW(TAG, "PIN entered but SIM not ready after multiple attempts");
        if (modem_send_command("AT", response, sizeof(response), MODEM_COMMAND_TIMEOUT) == ESP_OK) {
            ESP_LOGI(TAG, "Modem is still responding, but SIM may be having issues");
        } else {
            ESP_LOGE(TAG, "Modem not responding anymore - hardware issue likely");
        }
        return ESP_ERR_TIMEOUT;
    }
    
    return ESP_OK;
}

/**
 * @brief Send SMS message
 * 
 * @param phone_number Recipient phone number (with country code)
 * @param message SMS message text
 * @return esp_err_t ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t modem_send_sms(const char* phone_number, const char* message) {
    if (phone_number == NULL || message == NULL) {
        ESP_LOGE(TAG, "Invalid parameters for send_sms");
        return ESP_ERR_INVALID_ARG;
    }

    char cmd[64] = {0};
    char response[128] = {0};

    // Set SMS text mode
    if (modem_send_command("AT+CMGF=1", response, sizeof(response), MODEM_COMMAND_TIMEOUT) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set SMS text mode");
        return ESP_FAIL;
    }

    // Prepare the command to send the SMS
    snprintf(cmd, sizeof(cmd), "AT+CMGS=\"%s\"", phone_number);

    // Send the command and wait for the '>' prompt
    uart_write_bytes(MODEM_UART_NUM, cmd, strlen(cmd));
    uart_write_bytes(MODEM_UART_NUM, "\r", 1);

    uint32_t start_time = esp_timer_get_time() / 1000;
    bool prompt_received = false;
    memset(modem_buffer, 0, MODEM_BUFFER_SIZE);
    size_t total_bytes_read = 0;

    while ((esp_timer_get_time() / 1000 - start_time) < 5000) {
        uint8_t data;
        int len = uart_read_bytes(MODEM_UART_NUM, &data, 1, 100 / portTICK_PERIOD_MS);

        if (len > 0) {
            if (total_bytes_read < MODEM_BUFFER_SIZE - 1) {
                modem_buffer[total_bytes_read++] = data;
                modem_buffer[total_bytes_read] = '\0';
            }

            if (data == '>') {
                prompt_received = true;
                break;
            }
        }

        vTaskDelay(1);
    }

    if (!prompt_received) {
        ESP_LOGE(TAG, "Timeout waiting for SMS prompt");
        return ESP_ERR_TIMEOUT;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Send the SMS message
    uart_write_bytes(MODEM_UART_NUM, message, strlen(message));

    // Send the termination character (Ctrl+Z)
    uart_write_bytes(MODEM_UART_NUM, "\x1A", 1);

    // Wait for the final response
    uint32_t sms_start_time = esp_timer_get_time() / 1000;  // Renamed variable
    memset(modem_buffer, 0, MODEM_BUFFER_SIZE);
    size_t sms_total_bytes_read = 0;  // Renamed variable
    bool response_received = false;

    while ((esp_timer_get_time() / 1000 - sms_start_time) < 10000) {
        uint8_t data;
        int len = uart_read_bytes(MODEM_UART_NUM, &data, 1, 100 / portTICK_PERIOD_MS);

        if (len > 0) {
            if (sms_total_bytes_read < MODEM_BUFFER_SIZE - 1) {
                modem_buffer[sms_total_bytes_read++] = data;
                modem_buffer[sms_total_bytes_read] = '\0';
            }

            if (strstr(modem_buffer, AT_OK) || strstr(modem_buffer, "+CMGS")) {
                response_received = true;
                break;
            }
        }

        vTaskDelay(1);
    }

    if (response_received) {
        ESP_LOGI(TAG, "SMS sent successfully");
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to send SMS: %s", modem_buffer);
        return ESP_FAIL;
    }
}

void print_network_status(void) {
    char resp[128] = {0};
    if (modem_send_command("AT+CREG?", resp, sizeof(resp), MODEM_COMMAND_TIMEOUT) == ESP_OK) {
        int n, stat;
        if (sscanf(resp, "+CREG: %d,%d", &n, &stat) == 2) {
            const char* desc;
            switch (stat) {
                case 0: desc = "Not registered, not searching"; break;
                case 1: desc = "Registered (home)"; break;
                case 2: desc = "Not registered, searching"; break;
                case 3: desc = "Registration denied"; break;
                case 4: desc = "Unknown"; break;
                case 5: desc = "Registered (roaming)"; break;
                default: desc = "Unrecognized status"; break;
            }
            ESP_LOGI(TAG, "Network: %s → %s", resp, desc);
        } else {
            ESP_LOGW(TAG, "Can't parse CREG: %s", resp);
        }
    }
}

void print_signal_quality(void) {
    char resp[128] = {0};
    if (modem_send_command("AT+CSQ", resp, sizeof(resp), MODEM_COMMAND_TIMEOUT) == ESP_OK) {
        int rssi, ber;
        if (sscanf(resp, "+CSQ: %d,%d", &rssi, &ber) == 2) {
            int dbm;
            if (rssi == 0) dbm = -115;
            else if (rssi == 31) dbm = -52;
            else if (rssi == 99) dbm = 0;  // unknown
            else dbm = -113 + 2 * rssi;
            ESP_LOGI(TAG, "Signal: %s → %d dBm, BER=%d", resp, dbm, ber);
        } else {
            ESP_LOGW(TAG, "Can't parse CSQ: %s", resp);
        }
    }
}