#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void configure_modem(void);
esp_err_t modem_send_command(const char* command, char* response, size_t response_size, uint32_t timeout_ms);
esp_err_t modem_check_sim_status(void);
esp_err_t modem_enter_pin(const char* pin_code);
esp_err_t modem_send_sms(const char* phone_number, const char* message);
void print_network_status(void);
void print_signal_quality(void);

#ifdef __cplusplus
}
#endif