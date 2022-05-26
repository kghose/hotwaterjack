#include <driver/gpio.h>
#include "esp_timer.h"
#include "esp_log.h"

#include "sampler.h"

#define SAMPLE_DT 1000000
#define LED GPIO_NUM_2

static void configure_led(void)
{
    gpio_reset_pin(LED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

static void sample_data_callback(void *arg)
{
    gpio_set_level(LED, 1);    
    BoilerData *boiler_data = (BoilerData *)arg;
    uint8_t* row = next_writable_row(boiler_data);

    static uint8_t debug_idx = 0;
    static int incr = 1;
    if((debug_idx == 0) & (incr == -1)) incr = 1;
    if((debug_idx == 12) & (incr == 1)) incr = -1;
    row[0] = debug_idx;
    row[1] = debug_idx + 10;
    row[2] = debug_idx + 20;
    row[3] = debug_idx + 30;
    debug_idx += incr;
    gpio_set_level(LED, 0);
}

void start_sampler(BoilerData *boiler_data)
{
    configure_led();
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &sample_data_callback,
        .arg = (void *)boiler_data,
        .name = "sampler"};

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, SAMPLE_DT));
}