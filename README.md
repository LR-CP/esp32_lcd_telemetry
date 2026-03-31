# F1 LCD Display

## Synopsis
The goal of this project is to display streamed data from the F125 game to an I2C LCD display hooked up to my ESP32 Wrover MCU.
It will display simple telemtry data such as the RPM, current gear, speed, and current lap time.

## Hardware
- ESP32 Wrover MCU
- 1602 I2C LCD Display

## Setup
I am using the new `ESP-IDF Installation Manager (eim)` GUI software provided by Espressif to initialize the base `esp` repository and provide the necessary libraries. Within the tool, I press the 'open console' button which opens a terminal that has the PATH variables set for the esp-idf commands.  

Within the terminal, I run the following commands:
1. Create Project: `idf.py create-project f1_lcd_display && cd f1_lcd_display`
2. Set the target: `idf.py set-target esp32`
3. Configure params: `idf.py menuconfig` (not using any yet)
4. Build: `idf.py build`
5. Flash & Run: `idf.py -p PORT flash`

**NOTE:** Need to add user to dialout to flash board: `sudo usermod -a -G dialout $USER` and then logout and back in.

## Notes

### LCD Display
I am using the [1602 I2C LCD Display](https://www.handsontec.com/dataspecs/module/I2C_1602_LCD.pdf)  

The docs for integrating the I2C peripheral can be found [here](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/lcd/index.html)  

For now I am using the esp component made for this LCD display bu running `idf.py add-dependency "vgerwen/lcd1602^1.1.0"`