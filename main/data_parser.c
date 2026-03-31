#include "data_parser.h"

/**
 * Parses a Dirt Rally 2.0 packet and extracts dashboard data.
 * @param buf The input buffer containing the packet data.
 * @param len The length of the input buffer.
 * @param gear Pointer to store the extracted gear value.
 * @param speed_kmh Pointer to store the extracted speed in km/h.
 * @param rpm Pointer to store the extracted RPM value.
 * @return true if parsing is successful, false otherwise.
 */
bool parse_dirtrally2_packet(const uint8_t *buf, size_t len, int *gear, int *speed_kmh, int *rpm)
{
    if (buf == NULL || gear == NULL || speed_kmh == NULL || rpm == NULL)
    {
        ESP_LOGE("DataParser", "Null pointer provided for output parameters");
        return false;
    }
    
    if (len < 152) // Check if buffer is large enough to contain required data
    {
        ESP_LOGE("DataParser", "Buffer too small to parse");
        return false;
    }

    float speed;
    memcpy(&speed, &buf[28], sizeof(float));
    *speed_kmh = speed * 3.6f;

    float rpm_float;
    memcpy(&rpm_float, &buf[148], sizeof(float));
    *rpm = (int)rpm_float;

    float gear_float;
    memcpy(&gear_float, &buf[132], sizeof(float));
    *gear = (int)gear_float;

    return true;
}