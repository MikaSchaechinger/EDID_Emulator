#ifndef AT24CXX_H
#define AT24CXX_H



#include <eeprom/i2c.h>




class AT24CXX{

private:
    uint8_t address;  // I2C address of the EEPROM, but shifted one bit to the left
    uint16_t size;  // Size of the EEPROM in bytes


public: 
    AT24CXX(uint8_t address, uint16_t size);

    // The I2C address of the EEPROM (not shifted)
    void setI2CAddress(uint8_t address);

    // Get the I2C address of the EEPROM (not shifted)
    uint8_t getI2CAddress();
    
    uint8_t readOneByte(uint16_t addr);

    void writeOneByte(uint16_t addr, uint8_t data);

    void readBytes(uint16_t addr, uint8_t* buffer, uint16_t length);

    void writeBytes(uint16_t addr, uint8_t* buffer, uint16_t length);
};



#endif // AT24CXX_H