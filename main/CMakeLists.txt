idf_component_register(SRCS "lcddriver.c" "f1_lcd_display.c" "data_parser.c" "udp_server.c"
                    INCLUDE_DIRS "." REQUIRES driver esp_lcd nvs_flash esp_netif esp_wifi)
