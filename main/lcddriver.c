#include "lcddriver.h"

/**
- P0 = RS MSB
- P1 = RW
- P2 = E
- P3 = LED
- P4 = D4
- P5 = D5
- P6 = D6
- P7 = D7 LSB
 */

esp_err_t lcdInit(lcd_t *lcd, i2c_master_dev_handle_t dev)
{
    esp_err_t err;
    
    if (lcd == NULL || dev == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    lcd->dev_handle = dev;

    esp_rom_delay_us(41000); // Wait for more than 40ms after power on

    // Get into 4 bit mode
    err = lcd_write_nibble(lcd, 0x03, COMMAND);
    if (err != ESP_OK) return err;
    esp_rom_delay_us(4100);
    err = lcd_write_nibble(lcd, 0x03, COMMAND);
    if (err != ESP_OK) return err;
    esp_rom_delay_us(100);
    err = lcd_write_nibble(lcd, 0x03, COMMAND);
    if (err != ESP_OK) return err;
    esp_rom_delay_us(100);
    err = lcd_write_nibble(lcd, 0x02, COMMAND);
    if (err != ESP_OK) return err;
    esp_rom_delay_us(100);

    err = lcd_write_byte(lcd, 0x28, COMMAND); // Function set: 4-bit mode, 2 lines, 5x8 font
    if (err != ESP_OK) return err;
    err = lcd_write_byte(lcd, 0x08, COMMAND); // Display off, cursor off, blink off
    if (err != ESP_OK) return err;
    err = lcd_write_byte(lcd, 0x01, COMMAND); // Clear display
    if (err != ESP_OK) return err;
    err = lcd_write_byte(lcd, 0x06, COMMAND); // Entry mode set: increment cursor, no display shift
    if (err != ESP_OK) return err;
    err = lcd_write_byte(lcd, 0x0C, COMMAND); // Display on, cursor off, blink off
    if (err != ESP_OK) return err;
    return ESP_OK;
}

esp_err_t lcdWriteMessage(lcd_t* lcd, const char *message)
{
    if (lcd == NULL || message == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    int i = 0;
    while (message[i] != '\0')
    {
        if (message[i] == '\n')
        {
            esp_err_t err = lcd_write_byte(lcd, 0xC0, COMMAND); // Move cursor to second line
            if (err != ESP_OK)
            {
                return err;
            }
            i++;
            continue;
        }
        esp_err_t err = lcd_write_byte(lcd, message[i], DATA);
        if (err != ESP_OK)
        {
            return err;
        }
        i++;
    }
    return ESP_OK;
}

esp_err_t lcdClear(lcd_t *lcd)
{
    if (lcd == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    return lcd_write_byte(lcd, 0x01, COMMAND);
}

esp_err_t lcdSetCursor(lcd_t *lcd, uint8_t row, uint8_t col)
{
    if (lcd == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (row > 2 || col > 16 || row < 1 || col < 1)
    {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t row_offsets[] = {0x00, 0x40};
    // For row 2 column 6 it would be 0x40 + (col-1) = 0x45 and then 0x80 | 0x45 = 0xC5// For row 2 column 6 it would be 0x40 + 6 = 0x46 and then 0x80 | 0x46 = 0xC6
    uint8_t address = 0x80 | ((col-1) + row_offsets[row-1]);
    return lcd_write_byte(lcd, address, COMMAND);
}

void render_dashboard(lcd_t* lcd, int gear, int speed, int rpm)
{
    if (lcd == NULL) return;

    lcdSetCursor(lcd, 1, 4); // Gear location
    if (gear == -1)
    {
        lcdWriteMessage(lcd, " R");
    } else if (gear == 0)
    {
        lcdWriteMessage(lcd, " N");
    }
    else
    {
        char buf[3];
        snprintf(buf, sizeof(buf), "%2d", gear);
        lcdWriteMessage(lcd, buf);
    }


    lcdSetCursor(lcd, 1, 9); // RPM location
    char rpm_buf[9];
    snprintf(rpm_buf, sizeof(rpm_buf), "%5dRPM", rpm);
    lcdWriteMessage(lcd, rpm_buf);


    lcdSetCursor(lcd, 2, 10); // Speed location
    char speed_buf[8];
    snprintf(speed_buf, sizeof(speed_buf), "%2dKM/H", speed);
    lcdWriteMessage(lcd, speed_buf);
}

esp_err_t pcf8574_write(lcd_t *lcd, uint8_t data)
{
    return i2c_master_transmit(lcd->dev_handle, &data, 1, PCF8574_TIMEOUT_MS);
}

esp_err_t lcd_write_byte(lcd_t *lcd, uint8_t data, MODE mode)
{
    esp_err_t err;
    // If byte is 0x2 (0010), then upper nibble is 00 and lower is 01.
    uint8_t upper_nibble = (data >> 4) & 0x0F; // Shift right by 4 and mask to get upper nibble
    // Send upper nibble
    err = lcd_write_nibble(lcd, upper_nibble, mode);
    if (err != ESP_OK)
    {
        return err;
    }
    uint8_t lower_nibble = data & 0x0F; // Mask to get lower nibble
    // Send lower nibble
    err = lcd_write_nibble(lcd, lower_nibble, mode);
    if (err != ESP_OK)
    {
        return err;
    }
    if (mode == COMMAND && (data == 0x01 || data == 0x02)) // Clear display or return home commands
    {
        esp_rom_delay_us(2000);
    }
    else
    {
        esp_rom_delay_us(50);
    }
    return ESP_OK;
}

esp_err_t lcd_write_nibble(lcd_t *lcd, uint8_t nibble, MODE mode)
{
    esp_err_t err;
    uint8_t base = (nibble & 0x0F) << 4; // Shift nibble to upper 4 bits
    // Set RS pin based on mode (COMMAND or DATA)
    base |= LED_MASK; // Set LED bit high (P3) to turn on backlight
    if (mode == DATA)
    {
        base |= RS_MASK; // Set RS bit high for data
    }

    // Toggle E on and off to latch the data
    err = pcf8574_write(lcd, base); // Write data with E low to latch the nibble
    if (err != ESP_OK)
    {
        return err;
    }
    err = pcf8574_write(lcd, base | E_MASK); // Write data with E high
    if (err != ESP_OK)
    {
        return err;
    }
    err = pcf8574_write(lcd, base); // Write data with E low to latch the nibble
    if (err != ESP_OK)
    {
        return err;
    }
    return ESP_OK;
}