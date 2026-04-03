#include "esp_err.h"
#include "esp_rom_sys.h"
#include "esp_log.h"
#include "driver/gpio.h"

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

/**
 * Initialized the GPIO pins for the bar graph to outputs and sets them LOW
 * @param gpio_num An array of 10 GPIO numbers corresponding to the LEDs in the bar graph
 * @return ESP_OK on success, or an appropriate error code on failure
 */
esp_err_t led_init(gpio_num_t gpio_num[10]);

/**
 * Sets the state of a single LED
 * @param gpio_num The GPIO number of the LED to set
 * @param on True to turn the LED on, false to turn it off
 * @return ESP_OK on success, or an appropriate error code on failure
 */
esp_err_t led_set(gpio_num_t gpio_num, bool on);

/**
 * Turns off all LEDs in the bar graph
 * @param gpio_num An array of 10 GPIO numbers corresponding to the LEDs in the bar graph
 * @return ESP_OK on success, or an appropriate error code on failure
 */
esp_err_t led_clear_all(gpio_num_t gpio_num[10]);

/**
 * Sets the number of LEDs to light up based on the current RPM value relative to the maximum and idle RPM values.
 * The function calculates how many LEDs to light up based on the RPM range and updates the LED states accordingly.
 * @param gpio_num An array of 10 GPIO numbers corresponding to the LEDs in the bar graph
 * @param rpm The current RPM value to be displayed
 * @param max_rpm The maximum RPM value for scaling the LED display
 * @param idle_rpm The idle RPM value below which no LEDs should be lit
 * @return ESP_OK on success, or an appropriate error code on failure
 */
esp_err_t led_set_rpm(gpio_num_t gpio_num[10], int rpm, int max_rpm, int idle_rpm);

#endif // LED_CONTROL_H