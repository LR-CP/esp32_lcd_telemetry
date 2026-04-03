#include "led_control.h"

esp_err_t led_init(gpio_num_t gpio_num[10])
{
    for (int i =0; i < 10; i++)
    {
        esp_err_t err = gpio_set_direction(gpio_num[i], GPIO_MODE_OUTPUT);
        if (err != ESP_OK)
        { 
            ESP_LOGE("LED", "Failed to set GPIO direction for GPIO%d", gpio_num[i]);
            return err;
        }
        err = gpio_set_level(gpio_num[i], 0); // Start with all LEDs off
        if (err != ESP_OK)
        {
            ESP_LOGE("LED", "Failed to set GPIO level for GPIO%d", gpio_num[i]);
            return err; 
        }
    }

    return ESP_OK;
}

esp_err_t led_set(gpio_num_t gpio_num, bool on)
{
    return gpio_set_level(gpio_num, on ? 1 : 0);
}

esp_err_t led_clear_all(gpio_num_t gpio_num[10])
{
    for (int i = 0; i < 10; i++)
    {
        esp_err_t err = gpio_set_level(gpio_num[i], 0); // Turn off all LEDs
        if (err != ESP_OK)
        {
            ESP_LOGE("LED", "Failed to clear LED GPIO%d", gpio_num[i]);
            return err;
        }
    }
    return ESP_OK;
}

esp_err_t led_set_rpm(gpio_num_t gpio_num[10], int rpm, int max_rpm, int idle_rpm)
{
    esp_err_t err;
    int num_leds = 10; // Total number of LEDs (10 in this case)

    err = led_clear_all(gpio_num); // Clear all LEDs
    if (err != ESP_OK)
    {
        ESP_LOGE("LED", "Failed to clear LEDs before setting RPM");
        return err;
    }

    int range = max_rpm / num_leds; // RPM range per LED
    // Write a simple algorithm to determine how many LEDs to light up based on the current RPM
    int leds_to_light = 0;
    if (rpm <= idle_rpm)
    {
        leds_to_light = 0; // No LEDs for idle or below
    }
    else if (rpm >= max_rpm)
    {
        leds_to_light = num_leds; // All LEDs for max RPM or above
    }
    else
    {
        leds_to_light = (rpm - idle_rpm) / range; // Calculate how many LEDs to light based on RPM
    }

    for (int i = 0; i < leds_to_light; i++)
    {
        err = led_set(gpio_num[i], true); // Turn on the LED
        if (err != ESP_OK)
        {
            ESP_LOGE("LED", "Failed to set LED GPIO%d", gpio_num[i]);
            return err;
        }
    }

    return ESP_OK;
}