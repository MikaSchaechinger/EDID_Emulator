#include <eeprom/edid_manager.h>





EDIDManager::EDIDManager(GPIO_TypeDef* addrCtlPort, uint16_t addrCtlPin, GPIO_TypeDef* wpPort, uint16_t wpPin) : eeprom(EEPROM_ADDR_DEFAULT, EEPROM_SIZE) {
    // Set the address control pin
    this->addrCtlPort = addrCtlPort;
    this->addrCtlPin = addrCtlPin;
    this->wpPort = wpPort;
    this->wpPin = wpPin;
}


void EDIDManager::init() {
    // Set the address control pin to output
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = addrCtlPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(addrCtlPort, &GPIO_InitStructure);

    this->setDefaultAddress();
}


void EDIDManager::setDefaultAddress() {
    GPIO_ResetBits(addrCtlPort, addrCtlPin);
    eeprom.setI2CAddress(EEPROM_ADDR_DEFAULT);
}


void EDIDManager::setExtraAddress() {
    GPIO_SetBits(addrCtlPort, addrCtlPin);
    eeprom.setI2CAddress(EEPROM_ADDR_EXTRA);
}