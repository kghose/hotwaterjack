#include "esp_timer.h"
#include "esp_log.h"

#include "flashled.h"
#include "sampler.h"

#define SAMPLE_DT 500000

static const char *TAG = "Sampler";

void test_data_sampler(uint8_t *row)
{
    static uint8_t last_row[vars] = {0, 0, 0, 0};
    for (int i = 0; i < vars; i++)
    {
        if (last_row[i] == 255)
        {
            last_row[i] = 0;
        }
        else
        {
            last_row[i]++;
            break;
        }
    }
    for (int i = 0; i < vars; i++)
    {
        row[i] = last_row[i];
    }
}

void sample_data_callback(void *arg)
{
    flash_led(100000);
    BoilerData *boiler_data = (BoilerData *)arg;
    uint8_t *row = next_writable_row(boiler_data);
    test_data_sampler(row);
    // ESP_LOGI(TAG, "Data index: %d", boiler_data->last_index);
}

void start_sampler(BoilerData *boiler_data)
{
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &sample_data_callback,
        .arg = (void *)boiler_data,
        .name = "sampler"};

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, SAMPLE_DT));
}