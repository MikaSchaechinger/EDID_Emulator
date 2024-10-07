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

    // Set the write protect pin to output
    GPIO_InitStructure.GPIO_Pin = wpPin;
    GPIO_Init(wpPort, &GPIO_InitStructure);


    this->setDefaultAddress();
    this->writeProtect(false);

    this->cleanRamSlot();
    this->cleanRamMetaData();
}


EEPROM_ERROR EDIDManager::cleanSlot(uint8_t slot) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return EEPROM_ERROR::SLOT_OUT_OF_RANGE;
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();
    // Check if write protect is enabled
    u8 flags = this->getSlotMetaDataFlags(slot);
    if (flags & WRITE_PROTECT_MASK) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM_ERROR::SLOT_WRITE_PROTECT;
    }
    // Clean the slot in the EEPROM
    for (uint16_t i = 0; i < EDID_SLOT_SIZE; i++) 
        eeprom.writeOneByte(slot * EDID_SLOT_SIZE + i, 0);
    // Clean the meta data in the EEPROM
    for (uint16_t i = 0; i < EDID_META_DATA_SIZE; i++)
        eeprom.writeOneByte(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE + i, 0);
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();
    return EEPROM_ERROR::SUCCESS;
}


EEPROM_ERROR EDIDManager::cleanEEPROM() {
    // Check if the user has disabled the EEPROM protection
    if (this->clearProtectionPassword != EEPROM_CLEAR_PASSWORD) {
        return EEPROM_ERROR::EEPROM_CLEAR_PROTECTION;
    }
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();
    // Clean the EEPROM
    for (uint16_t i = 0; i < EEPROM_SIZE; i++) 
        eeprom.writeOneByte(i, 0);
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();
    return EEPROM_ERROR::SUCCESS;
}


EEPROM_ERROR EDIDManager::loadSlot(uint8_t slot) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return EEPROM_ERROR::SLOT_OUT_OF_RANGE;
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();   
    // Read the slot from the EEPROM
    this->slotToRAM(slot);
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();
    return EEPROM_ERROR::SUCCESS;
}


EEPROM_ERROR EDIDManager::saveSlot(uint8_t slot) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return EEPROM_ERROR::SLOT_OUT_OF_RANGE;
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();   
    // Check if write protect is enabled
    u8 flags = this->getSlotMetaDataFlags(slot);
    if (flags & WRITE_PROTECT_MASK) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM_ERROR::SLOT_WRITE_PROTECT;
    }
    // Write the slot to the EEPROM
    this->ramToSlot(slot);
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();
    return EEPROM_ERROR::SUCCESS;
}


EEPROM_ERROR EDIDManager::moveSlot(uint8_t slot, uint8_t targetSlot){
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT || targetSlot >= EDID_SLOT_COUNT) return EEPROM_ERROR::SLOT_OUT_OF_RANGE;
    // Abort if the slots are the same
    if (slot == targetSlot) return EEPROM_ERROR::SAME_SLOT;
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();
    // Check if write protect is enabled
    u8 flags = this->getSlotMetaDataFlags(targetSlot);
    if (flags & WRITE_PROTECT_MASK) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM_ERROR::SLOT_WRITE_PROTECT;
    }
    // Read the slot from the EEPROM
    this->slotToRAM(slot);
    // Write the slot to the EEPROM
    this->ramToSlot(targetSlot);
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();
    return EEPROM_ERROR::SUCCESS;
}


EEPROM_ERROR EDIDManager::applySlot(uint8_t slot){
    return this->moveSlot(slot, 0);     // Has all checks
}


EEPROM_ERROR EDIDManager::cloneToSlot(uint8_t slot) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return EEPROM_ERROR::SLOT_OUT_OF_RANGE;
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();
    // Check if write protect is enabled
    u8 flags = this->getSlotMetaDataFlags(slot);
    if (flags & WRITE_PROTECT_MASK) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM_ERROR::SLOT_WRITE_PROTECT;
    }
    bool success = this->readMonitorEDID();
    if (!success) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM_ERROR::MONITOR_NO_ANSWER;
    }
    // Write the slot in the RAM to the EEPROM
    this->ramToSlot(slot);
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();

    return EEPROM_ERROR::SUCCESS;
}





/************************* Private Functions *************************** */

void EDIDManager::setDefaultAddress() {
    GPIO_ResetBits(addrCtlPort, addrCtlPin);
    eeprom.setI2CAddress(EEPROM_ADDR_DEFAULT);
}


void EDIDManager::setExtraAddress() {
    GPIO_SetBits(addrCtlPort, addrCtlPin);
    eeprom.setI2CAddress(EEPROM_ADDR_EXTRA);
}


void EDIDManager::writeProtect(bool enable) {
    if (enable) {
        GPIO_SetBits(wpPort, wpPin);
    } else {
        GPIO_ResetBits(wpPort, wpPin);
    }
}


void EDIDManager::cleanRamSlot() {
    for (uint16_t i = 0; i < EDID_SLOT_SIZE; i++) {
        ramSlot.data[i] = 0;
    }
}


void EDIDManager::cleanRamMetaData() {
    for (uint16_t i = 0; i < 16; i++) {
        ramMetaData.data[i] = 0;
    }
}


void EDIDManager::slotToRAM(uint8_t slot) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return;
    // Read the slot from the EEPROM
    eeprom.readBytes(slot * EDID_SLOT_SIZE, ramSlot.data, EDID_SLOT_SIZE);
    // Read the meta data from the EEPROM
    eeprom.readBytes(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE, ramMetaData.data, EDID_META_DATA_SIZE);
}


void EDIDManager::ramToSlot(uint8_t slot) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return;
    // Write the slot to the EEPROM
    eeprom.writeBytes(slot * EDID_SLOT_SIZE, ramSlot.data, EDID_SLOT_SIZE);
    // Write the meta data to the EEPROM
    eeprom.writeBytes(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE, ramMetaData.data, EDID_META_DATA_SIZE);
}


u8 EDIDManager::getSlotMetaDataFlags(uint8_t slot) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return 0;
    // Read the first byte of the meta data from the eeeprom
    u8 flags = eeprom.readOneByte(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE);
    return flags;
}


void EDIDManager:: setSlotWriteProtect(uint8_t slot, bool enable) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return;
    // Read the first byte of the meta data from the eeeprom
    u8 flags = eeprom.readOneByte(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE);
    // Set the write protect bit
    if (enable) {
        flags |= WRITE_PROTECT_MASK;
    } else {
        flags &= ~WRITE_PROTECT_MASK;
    }
    // Write the flags back to the EEPROM
    eeprom.writeOneByte(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE, flags);
}









