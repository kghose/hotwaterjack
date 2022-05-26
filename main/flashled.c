#include <driver/gpio.h>
#include "esp_timer.h"

#include "flashled.h"

void configure_led(void)
{
    gpio_reset_pin(LED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

static void turn_off_led(void *arg)
{
    gpio_set_level(LED, 0);
}

void flash_led(size_t t)
{
    gpio_set_level(LED, 1);

    const esp_timer_create_args_t led_timer_args = {
        .callback = &turn_off_led,
        .name = "led"};

    esp_timer_handle_t led_timer;
    ESP_ERROR_CHECK(esp_timer_create(&led_timer_args, &led_timer));
    ESP_ERROR_CHECK(esp_timer_start_once(led_timer, t));
}
