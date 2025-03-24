
#include "eeprom/EEPROMAccessManager.h"

/**
 * @brief Manages Read/Write access to the EEPROM and the Monitor.
 * 
 * @param eeprom AT24CXX instance.
 * @param addrPort GPIO port of the address pin.
 * @param addrPin GPIO pin of the address pin.
 * 
 */
EEPROMAccessManager::EEPROMAccessManager(AT24CXX& eeprom, GPIO_TypeDef* addrPort, uint16_t addrPin)
    : eeprom(eeprom), addr_port(addrPort), addr_pin(addrPin){
    // A0 Pin als Ausgang
    GPIO_InitTypeDef gpio = {0};
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_10MHz;
    gpio.GPIO_Pin = addr_pin;
    GPIO_Init(addr_port, &gpio);
    selectBase0();
}


/**
 * @brief Controls the base address of the EEPROM by controlling a GPIO pin.
 * 
 * @param base1 If true, selects base address 1. If false, selects base address 0.
 */
void EEPROMAccessManager::selectBase(bool base1){
    if (base1)
        GPIO_SetBits(addr_port, addr_pin);
    else
        GPIO_ResetBits(addr_port, addr_pin);
}


/**
 * @brief Selects base address 0.
 */
void EEPROMAccessManager::selectBase0(){
    selectBase(false);
}


/**
 * @brief Selects base address 1.
 */
void EEPROMAccessManager::selectBase1(){
    selectBase(true);
}


/**
 * @brief Reads data from the Monitor.
 * 
 * @param addr Memory address to read from.
 * @param data Pointer to the buffer where the data will be stored.
 * @param len Number of bytes to read.
 * @return AT24CXX::Result Operation result. If I2C_Error, check AT24CXX::lastI2CResult() for details.
 */
AT24CXX::Result EEPROMAccessManager::readFromMonitor(uint16_t addr, uint8_t* data, uint16_t len){
    // Select Base 1, so the EEPROM has not the same address as the Monitor
    selectBase1();
    eeprom.setBusAddress(kMonitorAddress);
    AT24CXX::Result res = eeprom.readBytes(addr, data, len);
    selectBase0();
    return res;
}


/**
 * @brief Reads data from the EEPROM.
 * 
 * @param addr Memory address to read from.
 * @param data Pointer to the buffer where the data will be stored.
 * @param len Number of bytes to read.
 * 
 * @return AT24CXX::Result Operation result. If I2C_Error, check AT24CXX::lastI2CResult() for details.  
 */
AT24CXX::Result EEPROMAccessManager::readFromEEPROM(uint16_t addr, uint8_t* data, uint16_t len){
    selectBase1();
    eeprom.setBusAddress(kEEPROMAddress);
    AT24CXX::Result res = eeprom.readBytes(addr, data, len);
    selectBase0();
    return res;
}


/**
 * @brief Writes data to the EEPROM.
 * 
 * @param addr Memory address to write to.
 * @param data Pointer to the data to write.
 * @param len Number of bytes to write.
 * 
 * @return AT24CXX::Result Operation result. If I2C_Error, check AT24CXX::lastI2CResult() for details.
 */
AT24CXX::Result EEPROMAccessManager::writeToEEPROM(uint16_t addr, const uint8_t* data, uint16_t len){
    selectBase1();
    eeprom.setBusAddress(kEEPROMAddress);
    AT24CXX::Result res = eeprom.writeBytes(addr, data, len);
    selectBase0();
    return res;
}

















