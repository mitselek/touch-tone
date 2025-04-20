#include "settings.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "Settings";

// Save settings to NVS
esp_err_t save_settings(void) {
    esp_err_t err;
    nvs_handle_t nvs_handle;
    
    ESP_LOGI(TAG, "Saving settings...");
    
    // Open NVS with namespace "storage"
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS: %s", esp_err_to_name(err));
        return err;
    }
    
    // Save your settings here
    // Example: nvs_set_u8(nvs_handle, "brightness", brightness);
    
    // Commit changes to NVS
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error committing NVS: %s", esp_err_to_name(err));
    }
    
    // Close NVS
    nvs_close(nvs_handle);
    
    // Add a yield to prevent WDT issues during NVS operations
    vTaskDelay(1);
    
    return err;
}

// Load settings from NVS
esp_err_t load_settings(void) {
    esp_err_t err;
    nvs_handle_t nvs_handle;
    
    ESP_LOGI(TAG, "Loading settings...");
    
    // Open NVS with namespace "storage"
    err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGW(TAG, "NVS namespace not found, using defaults");
        } else {
            ESP_LOGE(TAG, "Error opening NVS: %s", esp_err_to_name(err));
        }
        return err;
    }
    
    // Load your settings here
    // Example:
    // uint8_t saved_brightness;
    // if (nvs_get_u8(nvs_handle, "brightness", &saved_brightness) == ESP_OK) {
    //     brightness = saved_brightness;
    // }
    
    // Close NVS
    nvs_close(nvs_handle);
    
    // Add a yield to prevent WDT issues during NVS operations
    vTaskDelay(1);
    
    return ESP_OK;
}