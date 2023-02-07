#include <stdio.h>
#include <conn/connectivity.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_netif.h>
#include <nvs_flash.h>
#include <esp_event.h>
#include <esp_err.h>

static void Setup(void);

void app_main(void)
{
    Setup();
    connectivity_SetMode(WIFI_MODE_STA);
    connectivity_SetStaCred("Typical", "Arghya@19");
    connectivity_SetApCred("ssid", "12345678");
    connectivity_StartWifi();
    connectivity_Connect(true);
}

void Setup(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}
