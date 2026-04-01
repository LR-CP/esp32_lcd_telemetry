#include <stdio.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "esp_system.h"

#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "lcddriver.h"
#include "data_parser.h"
#include "ledcontrol.h"

#define PORT 20777

typedef struct
{
    lcd_t *lcd;
    gpio_num_t *gpio_nums;
} udpParams;

void udp_listener(void *pvParameters)
{
    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0)
    {
        ESP_LOGE("UDP", "Failed to create socket");
        return;
    }

    // Cast the parameter to the correct type
    udpParams *params = (udpParams *)pvParameters;
    lcd_t *lcd = params->lcd;
    gpio_num_t *gpio_nums = params->gpio_nums;

    // Bind the socket to the specified port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        ESP_LOGE("UDP", "Failed to bind socket");
        close(sock);
        return;
    }

    ESP_LOGI("UDP", "UDP server setup complete on port %d", PORT);

    int len, gear, speed_kmh, rpm, max_rpm, idle_rpm;
    int last_gear = 0, last_speed_kmh = 0, last_rpm = 0;
    uint8_t rx_buffer[256];
    bool first_render = true;

    ESP_LOGI("UDP", "Waiting on recvfrom...");

    while(1)
    {
        len = recvfrom(sock, rx_buffer, sizeof(rx_buffer), 0, NULL, NULL);
        if (len < 0)
        {
            ESP_LOGE("UDP", "Failed to receive data");
            continue;
        }

        ESP_LOGE("recvfrom", "len = %d", len);

        if(parse_dirtrally2_packet(rx_buffer, len, &gear, &speed_kmh, &rpm, &max_rpm, &idle_rpm))
        {
            if (first_render)
            {
                lcdClear(lcd);
                render_dashboard(lcd, gear, speed_kmh, rpm);
                first_render = false;
                last_gear = gear;
                last_speed_kmh = speed_kmh;
                last_rpm = rpm;
            }
            else if (gear != last_gear || speed_kmh != last_speed_kmh || rpm != last_rpm)
            {
                render_dashboard(lcd, gear, speed_kmh, rpm);
                led_set_rpm(gpio_nums, rpm, max_rpm, idle_rpm);
                last_gear = gear;
                last_speed_kmh = speed_kmh;
                last_rpm = rpm;
            }
        }
        else
        {
            ESP_LOGW("UDP", "Received invalid packet");
        }
    }
}

/**
 * The main entry point of the application. It initializes the necessary components, sets up the I2C communication with the LCD display, and starts the UDP listener task to receive data from the Dirtrally2 game and update the LCD display accordingly.
 */
void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "CHANGEME",
            .password = "CHANGEME", // DO NOT KEEP THIS IS DUMB IM TIRED AND JUST WANNA TEST
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();
    vTaskDelay(pdMS_TO_TICKS(5000));  // 5 seconds

    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_get_ip_info(netif, &ip_info);

    ESP_LOGI("WIFI", "IP: " IPSTR, IP2STR(&ip_info.ip));

    /** I2C master bus configuration */
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0, // I2C port number (not used when auto selecting)
        .scl_io_num = GPIO_NUM_22,
        .sda_io_num = GPIO_NUM_21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x27,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    static lcd_t lcd;
    esp_err_t err;

    err = lcdInit(&lcd, dev_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("LCD", "Failed to initialize LCD");
        return;
    }

    err = lcdClear(&lcd);
    if (err != ESP_OK)
    {
        ESP_LOGE("LCD", "Failed to clear LCD");
        return;
    }

    err = lcdWriteMessage(&lcd, "Dirt Rally 2.0 Dash\nLucas Ricciardi");
    if (err != ESP_OK)
    {
        ESP_LOGE("LCD", "Failed to write message to LCD");
        return;
    }

    // pins I used: 18,32,33,14,12,13,5,0,2,15
    static gpio_num_t gpio_num[10] = {GPIO_NUM_33, GPIO_NUM_18, GPIO_NUM_32, GPIO_NUM_13, GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_0, GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_5};

    udpParams task_params = {
        .lcd = &lcd,
        .gpio_nums = gpio_num
    };

    err = led_init(gpio_num);
    if (err != ESP_OK)
    {
        ESP_LOGE("LED", "Failed to initialize LEDs");
        return;
    }

    xTaskCreate(udp_listener, "udp_listener", 4096, &task_params, 5, NULL);

    ESP_LOGV("LED", "LEDs initialized successfully");

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));  // 1 second
    }
}
