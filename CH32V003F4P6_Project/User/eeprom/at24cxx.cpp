#include "AT24CXX.h"
#include "I2C.h"
#include "ch32v00x_gpio.h"
#include "pins.h"
#include "time/SystemTimer.h"


/**
 * @brief Constructor.
 * 
 * @param addrPort GPIO port of the address line (A0).
 * @param addrPin Pin number of the address line (A0).
 * @param wpPort GPIO port of the write protect line (WP). If not used, set to nullptr.
 * @param wpPin Pin number of the write protect line (WP). If not used, set to 0.
 * 
 * @note The address line (A0) must be connected to the EEPROM. The write protect line (WP) is optional.
 */
AT24CXX::AT24CXX(uint16_t address, GPIO_TypeDef* wpPort, uint16_t wpPin)
    : address(address), wp_port(wpPort), wp_pin(wpPin){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // Port C global aktivieren


    // ggf. Write-Protect-Pin konfigurieren
    if (wp_port != nullptr) {
        
        // A0 Pin als Ausgang
        GPIO_InitTypeDef gpio = {0};
        gpio.GPIO_Mode = GPIO_Mode_Out_PP;
        gpio.GPIO_Speed = GPIO_Speed_10MHz;
        gpio.GPIO_Pin = wp_pin;
        GPIO_Init(wp_port, &gpio);
        setWriteProtect(true);  // Schreiben standardmäßig verbieten
    }
}


/**
 * @brief Sets the I2C bus address of the EEPROM.
 * 
 * @param address I2C bus address of the EEPROM.
 */
void AT24CXX::setBusAddress(uint16_t address){
    this->address = address;
}


/**
 * @brief Returns the I2C bus address of the EEPROM.
 * 
 * @return I2C bus address of the EEPROM.
 */
uint16_t AT24CXX::getBusAddress() const{
    return address;
}


/**
 * @brief Sets the write protect status.
 * 
 * Writes to the write protect pin of the EEPROM.
 * 
 * @param enable If true, write protect is enabled. If false, write protect is disabled.
 * 
 * @note If the write protect pin is not used, this function does nothing.
 */
void AT24CXX::setWriteProtect(bool enable){
    if (!wp_port) return;

    if (enable)
        GPIO_SetBits(wp_port, wp_pin);   // Write-Protect aktiv (HIGH)
    else
        GPIO_ResetBits(wp_port, wp_pin); // Write-Protect aus (LOW)
}


/**
 * @brief Returns the current write protect status.
 * 
 * @return true if write protect is enabled, false otherwise.
 * 
 * @note If the write protect pin is not used, always returns false.
 */
bool AT24CXX::getWriteProtect() const{
    if (!wp_port) return false;

    return GPIO_ReadOutputDataBit(wp_port, wp_pin);
}




/**
*   @brief Reads a number of bytes from the EEPROM.
*
*   Supports 1-byte and 2-byte addressing. Configure in AT24CXX_Config.h via "AT24CXX_USE_2BYTE_ADDR".
*
*   @param memAddr: Memory address to start reading from.
*   @param buffer: Pointer to the buffer where the data will be stored.
*   @param length: Number of bytes to read.
*   @return AT24CXX::Result Operation result. If I2C_Error, check lastI2CResult() for details.
*/
AT24CXX::Result AT24CXX::readBytes(uint16_t memAddr, uint8_t* buffer, uint16_t length){
    if (!buffer || length == 0 || memAddr + length > kEepromSize)
        return Result::InvalidParameter;

#if AT24CXX_USE_2BYTE_ADDR
    uint8_t addr[2] = {
        static_cast<uint8_t>((memAddr >> 8) & 0xFF), // MSB
        static_cast<uint8_t>(memAddr & 0xFF)         // LSB
    };

    I2C::Result res = I2C::write_then_read(
        static_cast<uint8_t>(address),
        addr, 2,
        buffer, length
    );
#else
    uint8_t addr = static_cast<uint8_t>(memAddr & 0xFF);

    I2C::Result res = I2C::write_then_read(
        static_cast<uint8_t>(address),
        &addr, 1,
        buffer, length
    );
#endif

    if (res != I2C::Result::Success) {
        last_i2c_result = res;
        return Result::I2C_Error;
    }

    return Result::Success;
}


/**
 * @brief Writes a number of bytes to the EEPROM.
 * 
 * Handles page boundaries automatically. Supports 1-byte and 2-byte addressing based on AT24CXX_Config.h.
 *
 * @param memAddr Start memory address to write to.
 * @param data Pointer to the data to write.
 * @param length Number of bytes to write.
 * @return AT24CXX::Result Operation result.
 */
AT24CXX::Result AT24CXX::writeBytes(uint16_t memAddr, const uint8_t* data, uint16_t length){
    if (!data || length == 0 || memAddr + length > kEepromSize)
        return Result::InvalidParameter;

    if (wp_port && GPIO_ReadOutputDataBit(wp_port, wp_pin)) {
        return Result::WriteProtectEnabled;
    }
    

    // AT24C32 supports Page Write (32 Bytes max)
    const uint8_t pageSize = 32;

    while (length > 0)
    {
        //uint8_t addr = static_cast<uint8_t>(memAddr & 0xFF);
        uint8_t pageOffset = memAddr % pageSize;
        uint8_t chunk = pageSize - pageOffset;
        if (chunk > length) chunk = length;

#if AT24CXX_USE_2BYTE_ADDR
        uint8_t temp[2 + pageSize];
        temp[0] = (memAddr >> 8) & 0xFF; // MSB
        temp[1] = memAddr & 0xFF;        // LSB
        for (uint8_t i = 0; i < chunk; ++i)
            temp[2 + i] = data[i];
        I2C::Result res = I2C::write(static_cast<uint8_t>(address), temp, chunk + 2);
#else
        uint8_t temp[1 + pageSize];
        temp[0] = memAddr & 0xFF;
        for (uint8_t i = 0; i < chunk; ++i)
            temp[1 + i] = data[i];
        I2C::Result res = I2C::write(static_cast<uint8_t>(address), temp, chunk + 1);
#endif

        if (res != I2C::Result::Success) {
            last_i2c_result = res;
            return Result::I2C_Error;
        }

        memAddr += chunk;
        data    += chunk;
        length  -= chunk;

        // EEPROM needs write time (~5ms)
        delay_ms(5);
    }

    return Result::Success;
}


/**
 * @brief Clears a region of EEPROM by writing a pattern (default: 0xFF).
 * 
 * Calls writeBytes() internally.
 *
 * @param memAddr Start address to clear.
 * @param length Number of bytes to write.
 * @param pattern Byte pattern to write (default: 0xFF).
 * @return AT24CXX::Result Operation result.
 */
AT24CXX::Result AT24CXX::clearBytes(uint16_t memAddr, uint16_t length, uint8_t pattern){
    if (length == 0 || memAddr + length > kEepromSize)
        return Result::InvalidParameter;

    const uint8_t chunkSize = 32;
    uint8_t buffer[chunkSize];

    for (uint8_t i = 0; i < chunkSize; ++i)
        buffer[i] = pattern;

    while (length > 0) {
        uint8_t chunk = (length > chunkSize) ? chunkSize : length;

        auto result = writeBytes(memAddr, buffer, chunk);
        if (result != Result::Success) {
            return result;
        }

        memAddr += chunk;
        length  -= chunk;
    }

    return Result::Success;
}

