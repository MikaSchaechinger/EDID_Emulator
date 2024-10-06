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


uint8_t AT24CXX::readOneByte(uint16_t addr)
{
    uint8_t temp = 0;
    // Wait until the I2C bus is no longer busy
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);
    // Generate a START condition
    I2C_GenerateSTART(I2C1, ENABLE);
    // Wait until START condition has been transmitted
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    // Send EEPROM address (use the class member 'address') in write mode (0xA0)
    I2C_Send7bitAddress(I2C1, this->address, I2C_Direction_Transmitter);
    // Wait until address has been acknowledged
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    // Send the memory address to read from
    I2C_SendData(I2C1, (uint8_t)(addr & 0x00FF));  // Send the LSB of the memory address
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    // Generate a RESTART condition to switch to read mode
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    // Send EEPROM address in read mode (0xA1)
    I2C_Send7bitAddress(I2C1, this->address, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    // Wait until the byte has been received
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
    // Disable ACK as we are reading only one byte
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    // Read the received byte
    temp = I2C_ReceiveData(I2C1);
    // Generate a STOP condition to end the transmission
    I2C_GenerateSTOP(I2C1, ENABLE);
    // Return the received byte
    return temp;
}


void AT24CXX::writeOneByte(uint16_t addr, uint8_t data)
{
    // Wait until the I2C bus is no longer busy
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);
    // Generate a START condition
    I2C_GenerateSTART(I2C1, ENABLE);
    // Wait until START condition has been transmitted
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    // Send EEPROM address (use the class member 'address') in write mode
    I2C_Send7bitAddress(I2C1, this->address, I2C_Direction_Transmitter);
    // Wait until address has been acknowledged
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    // Send the memory address to write to
    I2C_SendData(I2C1, (uint8_t)(addr & 0x00FF));  // Send the LSB of the memory address
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    // Send the data to be written
    I2C_SendData(I2C1, data);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    // Generate a STOP condition to end the transmission
    I2C_GenerateSTOP(I2C1, ENABLE);
}


void AT24CXX::readBytes(uint16_t addr, uint8_t* buffer, uint16_t length)
{
    // Wait until the I2C bus is no longer busy
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);

    // Generate a START condition
    I2C_GenerateSTART(I2C1, ENABLE);

    // Wait until START condition has been transmitted
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send EEPROM I2C address (already shifted) in write mode to set memory address
    I2C_Send7bitAddress(I2C1, this->address, I2C_Direction_Transmitter);

    // Wait until address has been acknowledged
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Send the memory address to read from
    I2C_SendData(I2C1, (uint8_t)(addr & 0x00FF));  // Send the LSB of the memory address
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Generate a RESTART condition to switch to read mode
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send EEPROM I2C address in read mode (already shifted with LSB for read mode)
    I2C_Send7bitAddress(I2C1, this->address | 1, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    // Read multiple bytes
    while (length)
    {
        if (length == 1) {
            // For the last byte, disable ACK and generate STOP
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        
        // Wait until the byte has been received
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
        
        // Store the received byte in the buffer
        *buffer = I2C_ReceiveData(I2C1);
        buffer++;  // Increment the buffer pointer
        length--;  // Decrement the remaining byte count
    }

    // Re-enable ACK for future read operations
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}


void AT24CXX::writeBytes(uint16_t addr, uint8_t* buffer, uint16_t length)
{
    // Wait until the I2C bus is no longer busy
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);

    // Generate a START condition
    I2C_GenerateSTART(I2C1, ENABLE);

    // Wait until START condition has been transmitted
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send EEPROM I2C address (already shifted) in write mode
    I2C_Send7bitAddress(I2C1, this->address, I2C_Direction_Transmitter);

    // Wait until address has been acknowledged
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Send the starting memory address to write to
    I2C_SendData(I2C1, (uint8_t)(addr & 0x00FF));  // Send the LSB of the memory address
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write multiple bytes to the EEPROM
    while (length--)
    {
        I2C_SendData(I2C1, *buffer);  // Send the byte from the buffer
        buffer++;  // Increment the buffer pointer

        // Wait until the byte has been transmitted
        while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    // Generate a STOP condition to end the transmission
    I2C_GenerateSTOP(I2C1, ENABLE);
}









