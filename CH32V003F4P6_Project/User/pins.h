#ifndef PIN_H
#define PIN_H

#include "ch32v00x_gpio.h"

// Port C Pins
#define I2C_SDA                 GPIO_Pin_1  // Read/Write EDID from EEPROM/Monitor
#define I2C_SCL                 GPIO_Pin_2  // Read/Write EDID from EEPROM/Monitor
#define EEPROM_WRITE_PROTECT    GPIO_Pin_3  // 1: writing disabled; 0: writing enabled (has a 10k Pull-Up Resistor)
#define EEPROM_ADDR_SEL         GPIO_Pin_4  // 1: Address is 0x51;  0: Address is 0x50
#define HDMI_VOLTAGE_DETECT     GPIO_Pin_5  // 1: HDMI Power is Active; 0 not HDMI Power detected. (forgot pull down resistor, so use internal)
#define SPI_MOSI                GPIO_Pin_6  // Not Used
#define SPI_MISO                GPIO_Pin_7  // Not Used


// Port D Pins
#define SWIO                    GPIO_Pin_1  // Programming Pin
#define BUTTON1                 GPIO_Pin_2  // User Button
#define STATUS_LED              GPIO_Pin_4  // Blue Status LED (low active)
#define RS232_TX                GPIO_Pin_5  // Serial communication over USB-C
#define RS232_RX                GPIO_Pin_6  // Serial communication over USB-C
#define ERROR_LED               GPIO_Pin_7  // Red Error LED (low active)


#endif // PIN_H
