#ifndef I2C_H
#define I2C_H


#include <pins.h>
#include <stdint.h>
#include <stddef.h>


/*********************************************************************
 * @fn      IIC_Init
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @return  none
 */
//void IIC_Init(u32 bound, u16 address);



class I2C {
    public:
    enum class Result {
        Success = 0,
        Busy,
        StartConditionError,
        SendAddressWriteError,
        SendMemoryAddressError,
        SendDataError,
        NackReceived,
        Timeout,

        RestartConditionError,
        SendAddressReadError,
        ReceiveDataError
    };

    // Initialisiert I2C mit gewünschter Frequenz in Hz
    static void init(uint32_t freq_hz = 100000);

    // Schreibt Daten an eine bestimmte I2C-Adresse
    static Result write(uint8_t device_addr, const uint8_t* data, size_t length);

    // Liest Daten von einer bestimmten I2C-Adresse
    static Result read(uint8_t device_addr, uint8_t* data, size_t length);

    // Kombiniert Write und Read (z. B. für EEPROMs)
    static Result write_then_read(uint8_t device_addr,
                                  const uint8_t* write_data, size_t write_len,
                                  uint8_t* read_data, size_t read_len);

private:
    static bool wait_for_flag(uint32_t flag, bool set, uint32_t timeout = 10000);
};


#endif // I2C_H
