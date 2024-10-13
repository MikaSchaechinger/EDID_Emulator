#include <eeprom/at24cxx.h>


AT24CXX::AT24CXX(uint8_t address, uint16_t size) {
    this->size = size;
    this->address = address;

    IIC_Init(100000, 0xA0);
}


void AT24CXX::setI2CAddress(uint8_t address) {
    this->address = address << 1;
}


uint8_t AT24CXX::getI2CAddress() {
    return this->address >> 1;
}


bool AT24CXX::waitForI2CEvent(uint32_t event) {
    uint32_t timeout = TIMEOUT_LIMIT;
    while (!I2C_CheckEvent(I2C1, event)) {
        if (timeout-- == 0) {
            return false;   // Timeout occurred
        }
    }
    return true;
}






I2C::State AT24CXX::readBytes(uint16_t addr, uint8_t* buffer, uint16_t length)
{
    // Wait until the I2C bus is no longer busy
    //while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);
    uint32_t timeout = TIMEOUT_LIMIT;
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET) {
        if (timeout-- == 0) {
            return I2C::BUSY_ERROR;   // Bus is busy for too long
        }
    }
    
    // Generate a START condition
    I2C_GenerateSTART(I2C1, ENABLE);

    // Wait until START condition has been transmitted
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_MODE_SELECT)) {
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }
    // Send EEPROM I2C address (already shifted) in write mode to set memory address
    I2C_Send7bitAddress(I2C1, this->address, I2C_Direction_Transmitter);

    // Wait until address has been acknowledged
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {  // Check if NACK received   
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);
            return I2C::NACK_ERROR;   // NACK received
        }
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }
    // Send the memory address to read from
    I2C_SendData(I2C1, (uint8_t)(addr & 0x00FF));  // Send the LSB of the memory address
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {  // Check if NACK received   
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);
            return I2C::NACK_ERROR;   // NACK received
        }
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }
    // Generate a RESTART condition to switch to read mode
    I2C_GenerateSTART(I2C1, ENABLE);
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_MODE_SELECT)) {
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }
    // Send EEPROM I2C address in read mode (already shifted with LSB for read mode)
    I2C_Send7bitAddress(I2C1, this->address | 1, I2C_Direction_Receiver);
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {  // Check if NACK received   
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);   // TODO: Necassary?
            return I2C::NACK_ERROR;   // NACK received
        }
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }

    // Read multiple bytes
    while (length)
    {
        if (length == 1) {
            // For the last byte, disable ACK and generate STOP
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        
        // Wait until the byte has been received
        timeout = TIMEOUT_LIMIT;
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET) {
            if (timeout-- == 0) {
                return I2C::TIMEOUT_ERROR;   // Timeout occurred
            }
        }

        // Store the received byte in the buffer
        *buffer = I2C_ReceiveData(I2C1);
        buffer++;  // Increment the buffer pointer
        length--;  // Decrement the remaining byte count
    }

    // Re-enable ACK for future read operations
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    return I2C::SUCCESS;  // Communication was successful
}


I2C::State AT24CXX::writeBytes(uint16_t addr, uint8_t* buffer, uint16_t length)
{
    // Wait until the I2C bus is no longer busy
    uint32_t timeout = TIMEOUT_LIMIT;
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET) {
        if (timeout-- == 0) {
            return I2C::BUSY_ERROR;   // Bus is busy for too long
        }
    }

    // Generate a START condition
    I2C_GenerateSTART(I2C1, ENABLE);

    // Wait until START condition has been transmitted
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_MODE_SELECT)) {
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }

    // Send EEPROM I2C address (already shifted) in write mode
    I2C_Send7bitAddress(I2C1, this->address, I2C_Direction_Transmitter);

    // Wait until address has been acknowledged
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {  // Check if NACK received   
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);  // Clear NACK flag
            return I2C::NACK_ERROR;   // NACK received, no device responded
        }
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }

    // Send the starting memory address to write to
    I2C_SendData(I2C1, (uint8_t)(addr & 0x00FF));  // Send the LSB of the memory address
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {  // Check if NACK received
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);  // Clear NACK flag
            return I2C::NACK_ERROR;   // NACK received
        }
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }

    // Write multiple bytes to the EEPROM
    while (length--) {
        I2C_SendData(I2C1, *buffer);  // Send the byte from the buffer
        buffer++;  // Increment the buffer pointer

        // Wait until the byte has been transmitted
        if (!this->waitForI2CEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
            if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {  // Check if NACK received
                I2C_ClearFlag(I2C1, I2C_FLAG_AF);  // Clear NACK flag
                return I2C::NACK_ERROR;   // NACK received
            }
            return I2C::TIMEOUT_ERROR;   // Timeout occurred
        }
    }

    // Generate a STOP condition to end the transmission
    I2C_GenerateSTOP(I2C1, ENABLE);

    return I2C::SUCCESS;  // Communication was successful
}


I2C::State AT24CXX::clearBytes(uint16_t addr, uint16_t length, uint8_t insert)
{
    // Wait until the I2C bus is no longer busy
    uint32_t timeout = TIMEOUT_LIMIT;
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET) {
        if (timeout-- == 0) {
            return I2C::BUSY_ERROR;   // Bus is busy for too long
        }
    }

    // Generate a START condition
    I2C_GenerateSTART(I2C1, ENABLE);

    // Wait until START condition has been transmitted
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_MODE_SELECT)) {
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }

    // Send EEPROM I2C address (already shifted) in write mode
    I2C_Send7bitAddress(I2C1, this->address, I2C_Direction_Transmitter);

    // Wait until address has been acknowledged
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {  // Check if NACK received   
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);  // Clear NACK flag
            return I2C::NACK_ERROR;   // NACK received, no device responded
        }
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }

    // Send the starting memory address to write to
    I2C_SendData(I2C1, (uint8_t)(addr & 0x00FF));  // Send the LSB of the memory address
    if (!this->waitForI2CEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {  // Check if NACK received
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);  // Clear NACK flag
            return I2C::NACK_ERROR;   // NACK received
        }
        return I2C::TIMEOUT_ERROR;   // Timeout occurred
    }

    // Write multiple bytes to the EEPROM
    while (length--) {
        I2C_SendData(I2C1, insert);  // Send insert byte

        // Wait until the byte has been transmitted
        if (!this->waitForI2CEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
            if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {  // Check if NACK received
                I2C_ClearFlag(I2C1, I2C_FLAG_AF);  // Clear NACK flag
                return I2C::NACK_ERROR;   // NACK received
            }
            return I2C::TIMEOUT_ERROR;   // Timeout occurred
        }
    }

    // Generate a STOP condition to end the transmission
    I2C_GenerateSTOP(I2C1, ENABLE);

    return I2C::SUCCESS;  // Communication was successful
}








