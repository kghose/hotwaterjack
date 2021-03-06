#include <driver/gpio.h>
#include "esp_timer.h"
#include "esp_log.h"

#include "ds18b20.h"

#include "flashled.h"
#include "sampler.h"

const int TEMP_BUS = GPIO_NUM_15; // The pin the DS18b20 is connected to
static const char *TAG = "Sampler";

void get_ds18b20_addrs(DeviceAddress *tempSensorAddresses, size_t *count)
{
    *count = 0;
    // This algorithm will freeze the ESP32 if the desired number of sensors are not hooked up
    // I had to use this because the response of my sensors could be intermittently glitchy
    // which means the algorithm below will sometimes not pull in all the sensors. We simply
    // rerun the search until we have a run where all the sensors are found.
    size_t expected_sensors = vars;
    while (*count < expected_sensors)
    {
        *count = 0;
        reset_search();
        // Search for addresses on the oneWire protocol
        // This algorithm will pull in just the number of devices seen on the bus,
        // and won't find all devices if some of them are glitching.
        while (search(tempSensorAddresses[*count], true))
        {
            (*count)++;
            if (*count == vars)
                break;
        }
    }
}

void initialize_temperature_sensors(BoilerData *boiler_data)
{
    ds18b20_init(TEMP_BUS);
    get_ds18b20_addrs((DeviceAddress *)boiler_data->tsensor_address, &(boiler_data->tsensor_count));
    ds18b20_setResolution((DeviceAddress *)boiler_data->tsensor_address, boiler_data->tsensor_count, 10);
    ESP_LOGI(TAG, "Found %d temp sensors", boiler_data->tsensor_count);
}

void test_data_sampler(uint8_t *row)
{
    static uint8_t last_row[vars] = {100, 120, 140, 160};
    for (int i = 0; i < vars; i++)
    {
        last_row[i] = last_row[i] < 200 ? last_row[i] + 1 : 100;
        row[i] = last_row[i];
    }
}

float get_raw_temp(const DeviceAddress *deviceAddress)
{
    ScratchPad scratchPad;
    if (ds18b20_isConnected(deviceAddress, scratchPad))
    {
        return calculateTemperature(deviceAddress, scratchPad);
    }
    return DEVICE_DISCONNECTED_RAW;
}

void sample_data_callback(void *arg)
{
    flash_led(100000);

    BoilerData *boiler_data = (BoilerData *)arg;
    int16_t *row = next_writable_row(boiler_data);
    // test_data_sampler(row); return;

    ds18b20_requestTemperatures();
    for (uint8_t i = 0; i < boiler_data->tsensor_count; i++)
    {
        row[i] = get_raw_temp((DeviceAddress *)boiler_data->tsensor_address[i]);
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
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, sample_dt_us));
}