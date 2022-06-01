#include <driver/gpio.h>
#include "esp_timer.h"
#include "esp_log.h"

#include "ds18b20.h"

#include "flashled.h"
#include "sampler.h"

#define SAMPLE_DT 1000000
const int TEMP_BUS = GPIO_NUM_15; // The pin the DS18b20 is connected to
static const char *TAG = "Sampler";

//DeviceAddress tempSensors[vars];

void get_ds18b20_addrs(DeviceAddress *tempSensorAddresses, size_t *count)
{
    *count = 0;
    reset_search();
    // search for addresses on the oneWire protocol
    while (search(tempSensorAddresses[*count], true))
    {
        (*count)++;
        if (*count == vars)
            break;
    }
}

void initialize_temperature_sensors(BoilerData *boiler_data)
{
    ds18b20_init(TEMP_BUS);
    get_ds18b20_addrs((DeviceAddress*)boiler_data->tsensor_address, &(boiler_data->tsensor_count));
    ds18b20_setResolution((DeviceAddress*)boiler_data->tsensor_address, boiler_data->tsensor_count, 10);
}

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
    // test_data_sampler(row);

    ds18b20_requestTemperatures();
    for(uint8_t i = 0 ; i < boiler_data->tsensor_count; i++) {
        float temp = ds18b20_getTempF((DeviceAddress *)boiler_data->tsensor_address[i]);
        row[i] = (int)temp;
    }
}

void start_sampler(BoilerData *boiler_data)
{
    initialize_temperature_sensors(boiler_data);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &sample_data_callback,
        .arg = (void *)boiler_data,
        .name = "sampler"};

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, SAMPLE_DT));
}