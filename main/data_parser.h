#include "esp_err.h"
#include "esp_rom_sys.h"
#include "esp_log.h"

#ifndef DATA_PARSER_H
#define DATA_PARSER_H

typedef struct dashboard_data
{
    int gear;
    int speed;
    int rpm;
} dashboard_data_t;

/**
 * Parses a Dirtrally2 packet and extracts the gear, speed, and RPM values.
 * @param buf The buffer containing the packet data.
 * @param len The length of the packet data.
 * @param gear Pointer to store the extracted gear value.
 * @param speed_kmh Pointer to store the extracted speed value in km/h.
 * @param rpm Pointer to store the extracted RPM value.
 * @param max_rpm Pointer to store the extracted maximum RPM value.
 * @param idle_rpm Pointer to store the extracted idle RPM value.
 * @return true if the packet was parsed successfully, false otherwise.
 */
bool parse_dirtrally2_packet(const uint8_t *buf, size_t len, int *gear, int *speed_kmh, int *rpm, int *max_rpm, int *idle_rpm);

#endif