#ifndef AT24CXX_H
#define AT24CXX_H



#include <eeprom/i2c.h>


#define TIMEOUT_LIMIT 4800000  // Timeout limit for I2C communication

namespace I2C{
    typedef enum{
    SUCCESS,        // No error occurred
    TIMEOUT_ERROR,  // Timeout occurred
    NACK_ERROR,     // Not acknowledge error
    BUSY_ERROR,     // Bus busy error
    UNKNOWN_ERROR   // Unknown error
    } State;
}




class AT24CXX{

private:
    uint8_t address;  // I2C address of the EEPROM, but shifted one bit to the left
    uint16_t size;  // Size of the EEPROM in bytes
    
    // Wait for an I2C event to occur. Returns true if the event occurred, false if a timeout occurred
    bool waitForI2CEvent(uint32_t event);


public: 
    // Address of the EEPROM and the size of the EEPROM in bytes
    AT24CXX(uint8_t address, uint16_t size);

    // The I2C address of the EEPROM (not shifted)
    void setI2CAddress(uint8_t address);

    // Get the I2C address of the EEPROM (not shifted)
    uint8_t getI2CAddress();
    
    I2C::State readBytes(uint16_t addr, uint8_t* buffer, uint16_t length);

    I2C::State writeBytes(uint16_t addr, uint8_t* buffer, uint16_t length);

    I2C::State clearBytes(uint16_t addr, uint16_t length, uint8_t insert=0);
};



#endif // AT24CXX_H