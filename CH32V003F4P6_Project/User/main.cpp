#include <stdint.h>
#include <debug.h>
#include "eeprom/AT24CXX.h"
//#include "I2C.h"
#include "time/SystemTimer.h"


// Testdaten
uint8_t test_data_base0[8] = { 11, 22, 33, 44, 55, 66, 77, 88 };
uint8_t test_data_base1[8] = { 88, 77, 66, 55, 44, 33, 22, 11 };
uint8_t read_buffer_0[8]   = { 0 };
uint8_t read_buffer_1[8]   = { 0 };

// Debug-Variablen
volatile AT24CXX::Result result_write_0;
volatile AT24CXX::Result result_read_0;
volatile AT24CXX::Result result_write_1;
volatile AT24CXX::Result result_read_1;

volatile I2C::Result last_i2c_result_0;
volatile I2C::Result last_i2c_result_1;

int main()
{
    // Init
    SystemTimer_Init();
    I2C::init(100000); // 100kHz

    // EEPROM: A0 = PC4, WP = PC3
    AT24CXX eeprom(GPIOC, EEPROM_ADDR_SEL, GPIOC, EEPROM_WRITE_PROTECT);

    eeprom.setWriteProtect(false);

    // --------- SLOT 0 (Adresse 0x50 / A0 = 0) ---------
    eeprom.selectBase0();
    result_write_0 = eeprom.writeBytes(0x00, test_data_base0, sizeof(test_data_base0));
    delay_ms(10);  // Sicherheitspuffer für Page Write
    result_read_0 = eeprom.readBytes(0x00, read_buffer_0, sizeof(read_buffer_0));
    last_i2c_result_0 = eeprom.lastI2CResult();

    // --------- SLOT 1 (Adresse 0x51 / A0 = 1) ---------
    eeprom.selectBase1();
    result_write_1 = eeprom.writeBytes(0x00, test_data_base1, sizeof(test_data_base1));
    delay_ms(10);
    result_read_1 = eeprom.readBytes(0x00, read_buffer_1, sizeof(read_buffer_1));
    last_i2c_result_1 = eeprom.lastI2CResult();

    
    eeprom.setWriteProtect(false);

    // 🐞 Breakpoint hier setzen → alles debuggen
    while (1) {
        // Optional: LED blinken oder Fehler signalisieren
    }
}
