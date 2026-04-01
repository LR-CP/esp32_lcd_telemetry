#include <stdint.h>
#include "esp_err.h"
#include "esp_rom_sys.h"
#include "esp_log.h"
#include "driver/gpio.h"

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

esp_err_t led_init(gpio_num_t gpio_num[10]);
esp_err_t led_set(gpio_num_t gpio_num, bool on);
esp_err_t led_clear_all(gpio_num_t gpio_num[10]);
esp_err_t led_set_rpm(gpio_num_t gpio_num[10], int rpm, int max_rpm, int idle_rpm);

#endif // LED_CONTROL_H