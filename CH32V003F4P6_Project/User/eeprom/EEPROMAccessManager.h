#ifndef EEPROM_ACCESS_MANAGER_H
#define EEPROM_ACCESS_MANAGER_H


#include "eeprom/AT24CXX.h"


class EEPROMAccessManager {
public:
    EEPROMAccessManager(AT24CXX& eeprom, GPIO_TypeDef* addrPort, uint16_t addrPin);

    static constexpr uint8_t kMonitorAddress = 0x50;
    static constexpr uint8_t kEEPROMAddress = 0x51;

    AT24CXX::Result readFromMonitor(uint16_t addr, uint8_t* data, uint16_t len);
    AT24CXX::Result readFromEEPROM(uint16_t addr, uint8_t* data, uint16_t len);
    AT24CXX::Result writeToEEPROM(uint16_t addr, const uint8_t* data, uint16_t len);

private:

    AT24CXX& eeprom;
    GPIO_TypeDef* addr_port;
    uint16_t addr_pin;

    // Selects the base address for the EEPROM.
    void selectBase(bool base1);
    void selectBase0();
    void selectBase1();

};


#endif // EEPROM_ACCESS_MANAGER_H