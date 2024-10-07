#ifndef AT24CXX_H
#define AT24CXX_H



#include <eeprom/i2c.h>


#define TIMEOUT_LIMIT 4800000  // Timeout limit for I2C communication

typedef enum{
    SUCCESS,
    TIMEOUT_ERROR
} COMMUNICATION_ERROR;



class AT24CXX{

private:
    uint8_t address;  // I2C address of the EEPROM, but shifted one bit to the left
    uint16_t size;  // Size of the EEPROM in bytes


public: 
    // Address of the EEPROM and the size of the EEPROM in bytes
    AT24CXX(uint8_t address, uint16_t size);

    // The I2C address of the EEPROM (not shifted)
    void setI2CAddress(uint8_t address);

    // Get the I2C address of the EEPROM (not shifted)
    uint8_t getI2CAddress();
    
    uint8_t readOneByte(uint16_t addr);

    COMMUNICATION_ERROR writeOneByte(uint16_t addr, uint8_t data);

    COMMUNICATION_ERROR readBytes(uint16_t addr, uint8_t* buffer, uint16_t length);

    COMMUNICATION_ERROR writeBytes(uint16_t addr, uint8_t* buffer, uint16_t length);
};



#endif // AT24CXX_H