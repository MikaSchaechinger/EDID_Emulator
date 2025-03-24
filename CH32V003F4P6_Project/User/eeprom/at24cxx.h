#ifndef AT24CXX_H
#define AT24CXX_H

#include "i2c.h"
#include "ch32v00x_gpio.h"
#include "AT24CXX_Config.h"

// Change in AT24CXX_Config.h   
#if AT24CXX_USE_2BYTE_ADDR              
    #define AT24CXX_ADDR_BYTES 2
#else
    #define AT24CXX_ADDR_BYTES 1
#endif



class AT24CXX {
public:
    enum class Result {
        Success,
        I2C_Error,
        WriteProtectEnabled,
        InvalidParameter,
        UnknownError
    };



    AT24CXX(uint16_t address,
            GPIO_TypeDef* wpPort = nullptr, uint16_t wpPin = 0);

    void setBusAddress(uint16_t address);
    uint16_t getBusAddress() const;

    void setWriteProtect(bool enable);
    bool getWriteProtect() const;  

    Result readBytes(uint16_t memAddr, uint8_t* buffer, uint16_t length);
    Result writeBytes(uint16_t memAddr, const uint8_t* data, uint16_t length);
    Result clearBytes(uint16_t memAddr, uint16_t length, uint8_t pattern = 0xFF);


    I2C::Result lastI2CResult() const { return last_i2c_result; }

    static constexpr uint16_t kEepromSize = AT24CXX_SIZE_BYTES; // Change in AT24CXX_Config.h

private:
    uint16_t address;

    GPIO_TypeDef* wp_port;
    uint16_t wp_pin;

    I2C::Result last_i2c_result = I2C::Result::Success;
};

#endif // AT24CXX_H
