#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "server.h"
#include "data.h"
#include "flashled.h"
#include "sampler.h"

static const char *TAG = "Hot Water Jack";

void app_main(void)
{
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(esp_event_loop_create_default());

  wifi_init_sta();

  BoilerData *boiler_data = calloc(1, sizeof(BoilerData));
  ESP_LOGI(TAG, "Memory allocated for data: %d", sizeof(BoilerData));
  ESP_LOGI(TAG, "Free memory: %d", xPortGetFreeHeapSize());

  start_data_server(boiler_data);
  start_sampler(boiler_data);
}
