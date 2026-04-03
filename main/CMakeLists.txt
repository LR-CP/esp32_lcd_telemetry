idf_component_register(SRCS "led_control.c" "lcd_driver.c" "main.c" "data_parser.c" "udp_server.c"
                    INCLUDE_DIRS "." REQUIRES driver esp_driver_gpio esp_lcd nvs_flash esp_netif esp_wifi)
