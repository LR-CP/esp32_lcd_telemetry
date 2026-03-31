#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"
#include "esp_rom_sys.h"
#include "esp_log.h"

#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#define RS_MASK 0x01  // P0
#define RW_MASK 0x02  // P1
#define E_MASK 0x04   // P2
#define LED_MASK 0x08 // P3

#define PCF8574_TIMEOUT_MS 50

typedef struct lcd
{
    i2c_master_dev_handle_t dev_handle;
} lcd_t;

typedef enum MODE
{
    COMMAND = 0, // LOW
    DATA = 1     // HIGH
} MODE;

/* This library provides an API for the 1602 LCD display provided in the freenove starter kit */

/**
 * Initializes the LCD display by sending the necessary commands to set it up in 4-bit mode, configure the display settings, and clear the screen.
 * @param lcd Pointer to the lcd_t structure representing the LCD device.
 * @param dev The I2C master device handle for communication with the LCD.
 * @return ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t lcdInit(lcd_t *lcd, i2c_master_dev_handle_t dev);

/**
 * Writes a null-terminated string message to the LCD display. The message can contain newline characters to indicate line breaks.
 * @param lcd Pointer to the lcd_t structure representing the LCD device.
 * @param message The null-terminated string message to be displayed on the LCD.
 * @return ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t lcdWriteMessage(lcd_t* lcd, const char *message);

/**
 * Clears the LCD display by sending the appropriate command to the LCD controller.
 * @param lcd Pointer to the lcd_t structure representing the LCD device.
 * @return ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t lcdClear(lcd_t *lcd);

/**
 * Sets the cursor position on the LCD display to the specified row and column. The top-left corner of the display is considered as row 1, column 1.
 * @param lcd Pointer to the lcd_t structure representing the LCD device.
 * @param row The row number (1 or 2) where the cursor should be positioned.
 * @param col The column number (1 to 16) where the cursor should be positioned.
 * @return ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t lcdSetCursor(lcd_t *lcd, uint8_t row, uint8_t col);

/**
 * Renders the dashboard data (gear, speed, and RPM) on the LCD display in a formatted manner. The gear is displayed on the first line, while the speed and RPM are displayed on the second line.
 * @param lcd Pointer to the lcd_t structure representing the LCD device.
 * @param gear The current gear value to be displayed. A value of -1 indicates reverse gear, 0 indicates neutral, and positive values indicate forward gears.
 * @param speed The current speed in km/h to be displayed.
 * @param rpm The current RPM value to be displayed.
 */
void render_dashboard(lcd_t* lcd, int gear, int speed, int rpm); // Not sure if i want to keep in driver but it is a nice example of how to use the API

/**
 * Writes a single byte of data to the PCF8574 I/O expander, which is used to control the LCD display. The function sends the byte over I2C and handles any necessary timing requirements.
 * @param lcd Pointer to the lcd_t structure representing the LCD device.
 * @param data The byte of data to be written to the PCF8574. Each bit in the byte corresponds to a specific pin on the I/O expander, which controls various functions of the LCD (e.g., RS, RW, E, LED, D4-D7).    
 */
esp_err_t pcf8574_write(lcd_t* lcd, uint8_t data);

/**
 * Writes a single byte of data to the LCD display.
 * @param lcd Pointer to the lcd_t structure representing the LCD device.
 * @param data The byte of data to be written to the LCD.
 * @param mode The mode in which to write the data (COMMAND or DATA).
 * @return ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t lcd_write_byte(lcd_t* lcd, uint8_t data, MODE mode);

/**
 * Writes a 4-bit nibble to the LCD display. This function is used internally by lcd_write_byte to send the upper and lower nibbles of a byte separately, as the LCD is configured in 4-bit mode.
 * @param lcd Pointer to the lcd_t structure representing the LCD device.
 * @param nibble The 4-bit nibble to be written to the LCD (should be in the lower 4 bits of the byte).
 * @param mode The mode in which to write the nibble (COMMAND or DATA).
 * @return ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t lcd_write_nibble(lcd_t* lcd, uint8_t nibble, MODE mode);

#endif // LCD_DRIVER_H