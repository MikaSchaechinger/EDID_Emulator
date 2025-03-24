#include <eeprom/i2c.h>
#include <pins.h>


void I2C::init(uint32_t freq_hz)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitStructure = {0};

    // Clock für GPIOC und AFIO aktivieren
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // SCL auf PC2
    GPIO_InitStructure.GPIO_Pin = I2C_SCL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SDA auf PC1
    GPIO_InitStructure.GPIO_Pin = I2C_SDA;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // I2C konfigurieren
    I2C_InitStructure.I2C_ClockSpeed = freq_hz;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitStructure.I2C_OwnAddress1 = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}


bool I2C::wait_for_flag(uint32_t event, bool set, uint32_t timeout)
{
    while ((I2C_CheckEvent(I2C1, event) == (set ? RESET : SET)) && timeout--) {
        // evtl. kleine Delay-Schleife für weniger CPU-Last
    }
    return timeout > 0;
}


#include "I2C.h"
#include "ch32v00x_i2c.h"

#define TIMEOUT_LIMIT 10000

I2C::Result I2C::write_then_read(uint8_t device_addr,
                                 const uint8_t* write_data, size_t write_len,
                                 uint8_t* read_data, size_t read_len)
{
    uint32_t timeout;

    // Warten bis Bus frei ist
    timeout = TIMEOUT_LIMIT;
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET) {
        if (--timeout == 0) return Result::Busy;
    }

    // START senden
    I2C_GenerateSTART(I2C1, ENABLE);
    if (!wait_for_flag(I2C_EVENT_MASTER_MODE_SELECT, true)) {
        return Result::StartConditionError;
    }

    // Adresse im Write-Modus senden
    I2C_Send7bitAddress(I2C1, device_addr, I2C_Direction_Transmitter);
    if (!wait_for_flag(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, true)) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF)) {
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);
            return Result::NackReceived;
        }
        return Result::SendAddressWriteError;
    }

    // Daten senden (z. B. EEPROM-Speicheradresse)
    for (size_t i = 0; i < write_len; ++i) {
        I2C_SendData(I2C1, write_data[i]);
        if (!wait_for_flag(I2C_EVENT_MASTER_BYTE_TRANSMITTED, true)) {
            return Result::SendDataError;
        }
    }

    // RESTART für Read-Modus
    I2C_GenerateSTART(I2C1, ENABLE);
    if (!wait_for_flag(I2C_EVENT_MASTER_MODE_SELECT, true)) {
        return Result::RestartConditionError;
    }

    // Adresse im Read-Modus senden
    I2C_Send7bitAddress(I2C1, device_addr, I2C_Direction_Receiver);
    if (!wait_for_flag(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, true)) {
        return Result::SendAddressReadError;
    }

    // Empfangsschleife
    while (read_len) {
        if (read_len == 1) {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }

        timeout = TIMEOUT_LIMIT;
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET) {
            if (--timeout == 0) return Result::ReceiveDataError;
        }

        *read_data++ = I2C_ReceiveData(I2C1);
        --read_len;
    }

    I2C_AcknowledgeConfig(I2C1, ENABLE);
    return Result::Success;
}


I2C::Result I2C::write(uint8_t device_addr, const uint8_t* data, size_t length)
{
    uint32_t timeout = TIMEOUT_LIMIT;

    // Warten bis Bus frei ist
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
        if (--timeout == 0) return Result::Busy;
    }

    // START senden
    I2C_GenerateSTART(I2C1, ENABLE);
    if (!wait_for_flag(I2C_EVENT_MASTER_MODE_SELECT, true)) {
        return Result::StartConditionError;
    }

    // Adresse im Write-Modus senden
    I2C_Send7bitAddress(I2C1, device_addr, I2C_Direction_Transmitter);
    if (!wait_for_flag(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, true)) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF)) {
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);
            return Result::NackReceived;
        }
        return Result::SendAddressWriteError;
    }

    // Daten senden
    for (size_t i = 0; i < length; ++i) {
        I2C_SendData(I2C1, data[i]);
        if (!wait_for_flag(I2C_EVENT_MASTER_BYTE_TRANSMITTED, true)) {
            return Result::SendDataError;
        }
    }

    // STOP senden
    I2C_GenerateSTOP(I2C1, ENABLE);

    return Result::Success;
}



I2C::Result I2C::read(uint8_t device_addr, uint8_t* buffer, size_t length)
{
    uint32_t timeout = TIMEOUT_LIMIT;

    // Warten bis Bus frei ist
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
        if (--timeout == 0) return Result::Busy;
    }

    // START senden
    I2C_GenerateSTART(I2C1, ENABLE);
    if (!wait_for_flag(I2C_EVENT_MASTER_MODE_SELECT, true)) {
        return Result::StartConditionError;
    }

    // Adresse im Read-Modus senden
    I2C_Send7bitAddress(I2C1, device_addr, I2C_Direction_Receiver);
    if (!wait_for_flag(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, true)) {
        return Result::SendAddressReadError;
    }

    // Empfangsschleife
    while (length)
    {
        if (length == 1) {
            I2C_AcknowledgeConfig(I2C1, DISABLE);  // Kein ACK nach letztem Byte
            I2C_GenerateSTOP(I2C1, ENABLE);        // STOP
        }

        timeout = TIMEOUT_LIMIT;
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET) {
            if (--timeout == 0) return Result::ReceiveDataError;
        }

        *buffer++ = I2C_ReceiveData(I2C1);
        --length;
    }

    I2C_AcknowledgeConfig(I2C1, ENABLE); // Wieder aktivieren

    return Result::Success;
}



/*void IIC_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    I2C_InitTypeDef I2C_InitTSturcture={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE );

    // I2C SCL
    GPIO_InitStructure.GPIO_Pin = I2C_SCL;              // PD2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    // I2C SDA
    GPIO_InitStructure.GPIO_Pin = I2C_SDA;              // PD1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init( I2C1, &I2C_InitTSturcture );

    I2C_Cmd( I2C1, ENABLE );

    I2C_AcknowledgeConfig( I2C1, ENABLE );
}*/
