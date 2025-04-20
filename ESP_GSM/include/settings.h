#pragma once
#include "esp_err.h"

// Function to save settings to non-volatile storage
esp_err_t save_settings(void);

// Function to load settings from non-volatile storage
esp_err_t load_settings(void);