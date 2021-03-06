#include <driver/gpio.h>
#include "esp_timer.h"

#include "flashled.h"

#define LED GPIO_NUM_2

static int configured = 0;
static int state = 0;
esp_timer_handle_t led_timer;

void configure_led(void)
{
    gpio_reset_pin(LED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    configured = 1;
}

static void turn_off_led(void *arg)
{
    gpio_set_level(LED, 0);
    state = 0;
    esp_timer_delete(led_timer);
}

void flash_led(size_t t)
{
    if (!configured) configure_led();

    if(state) // Someone already turned the LED on
    {
        esp_timer_stop(led_timer);
        esp_timer_delete(led_timer);
    }

    gpio_set_level(LED, 1);
    state = 1;
    const esp_timer_create_args_t led_timer_args = {
        .callback = &turn_off_led,
        .name = "led"};

    ESP_ERROR_CHECK(esp_timer_create(&led_timer_args, &led_timer));
    ESP_ERROR_CHECK(esp_timer_start_once(led_timer, t));
}
