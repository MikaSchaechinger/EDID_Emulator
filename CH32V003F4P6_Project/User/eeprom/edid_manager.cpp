#include <eeprom/edid_manager.h>





EDIDManager::EDIDManager(GPIO_TypeDef* addrCtlPort, uint16_t addrCtlPin, GPIO_TypeDef* wpPort, uint16_t wpPin) : eeprom(EEPROM_ADDR_DEFAULT, EEPROM_SIZE) {
    // Set the address control pin
    this->addrCtlPort = addrCtlPort;
    this->addrCtlPin = addrCtlPin;
    this->wpPort = wpPort;
    this->wpPin = wpPin;
    this->clearProtectionPassword = 0x00000000;
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


EEPROM::State EDIDManager::cleanSlot(uint8_t slot, bool deepClean) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return EEPROM::State::SLOT_OUT_OF_RANGE;
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();
    // Check if write protect is enabled
    u8 flags;
    EEPROM::State error = this->getSlotMetaDataFlags(slot, &flags);
    if (error != EEPROM::State::SUCCESS) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return error;
    }
    if (flags & WRITE_PROTECT_MASK) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM::State::SLOT_WRITE_PROTECT;
    }
    // Deep clean: Clean the slot and the meta data from the EEPROM
    I2C::State status;
    if (deepClean) {
        // Clean the slot in the EEPROM
        status = eeprom.clearBytes(slot * EDID_META_DATA_SIZE, EDID_META_DATA_SIZE);
        if (status != I2C::State::SUCCESS) {
            this->setDefaultAddress();
            return EEPROM::State::COMMUNICATION_ERROR;
        }
    }
    // Clean the meta data in the EEPROM
    status = eeprom.clearBytes(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE, EDID_META_DATA_SIZE);
    if (status != I2C::State::SUCCESS) {
        this->setDefaultAddress();
        return EEPROM::State::COMMUNICATION_ERROR;
    }
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();
    return EEPROM::State::SUCCESS;
}


EEPROM::State EDIDManager::cleanEEPROM() {
    // Check if the user has disabled the EEPROM protection
    if (this->clearProtectionPassword != EEPROM_CLEAR_PASSWORD) {
        return EEPROM::State::EEPROM_CLEAR_PROTECTION;
    }
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();
    // Clean the EEPROM
    I2C::State status = eeprom.clearBytes(0, EEPROM_SIZE);
    if (status != I2C::State::SUCCESS) {
        this->setDefaultAddress();
        return EEPROM::State::COMMUNICATION_ERROR;
    }
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();
    return EEPROM::State::SUCCESS;
}


EEPROM::State EDIDManager::loadSlot(uint8_t slot) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return EEPROM::State::SLOT_OUT_OF_RANGE;
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();   
    // Read the slot from the EEPROM
    this->slotToRAM(slot);
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();
    return EEPROM::State::SUCCESS;
}


EEPROM::State EDIDManager::saveSlot(uint8_t slot) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return EEPROM::State::SLOT_OUT_OF_RANGE;
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();   
    // Check if write protect is enabled
    u8 flags;
    EEPROM::State status = this->getSlotMetaDataFlags(slot, &flags);
    if (status != EEPROM::State::SUCCESS) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return status;
    }
    if (flags & WRITE_PROTECT_MASK) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM::State::SLOT_WRITE_PROTECT;
    }
    // Write the slot to the EEPROM
    this->ramToSlot(slot);
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();
    return EEPROM::State::SUCCESS;
}


EEPROM::State EDIDManager::moveSlot(uint8_t slot, uint8_t targetSlot){
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT || targetSlot >= EDID_SLOT_COUNT) return EEPROM::State::SLOT_OUT_OF_RANGE;
    // Abort if the slots are the same
    if (slot == targetSlot) return EEPROM::State::SAME_SLOT;
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();
    // Check if write protect is enabled
    u8 flags;
    EEPROM::State status = this->getSlotMetaDataFlags(targetSlot, &flags);
    if (status != EEPROM::State::SUCCESS) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return status;
    }
    if (flags & WRITE_PROTECT_MASK) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM::State::SLOT_WRITE_PROTECT;
    }
    // Read the slot from the EEPROM
    this->slotToRAM(slot);
    // Write the slot to the EEPROM
    this->ramToSlot(targetSlot);
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();
    return EEPROM::State::SUCCESS;
}


EEPROM::State EDIDManager::applySlot(uint8_t slot){
    return this->moveSlot(slot, 0);     // Has all checks
}


EEPROM::State EDIDManager::cloneToSlot(uint8_t slot) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return EEPROM::State::SLOT_OUT_OF_RANGE;
    // Apply the extra address, so there is no conflict with the I2C PC - Monitor
    this->setExtraAddress();
    // Check if write protect is enabled
    u8 flags;
    EEPROM::State status = this->getSlotMetaDataFlags(slot, &flags);
    if (status != EEPROM::State::SUCCESS) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return status;
    }
    if (flags & WRITE_PROTECT_MASK) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM::State::SLOT_WRITE_PROTECT;
    }
    bool success = this->readMonitorEDID();
    if (!success) {
        // Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM::State::MONITOR_NO_ANSWER;
    }
    // Write the slot in the RAM to the EEPROM
    this->ramToSlot(slot);
    // Apply the default address, so the PC can read the EDID
    this->setDefaultAddress();

    return EEPROM::State::SUCCESS;
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


EEPROM::State EDIDManager::getSlotMetaDataFlags(uint8_t slot, u8* flags) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return EEPROM::State::SLOT_OUT_OF_RANGE;
    // Read the first byte of the meta data from the eeeprom
    I2C::State status;
    status = eeprom.readBytes(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE, flags, 1);
    if (status != I2C::State::SUCCESS) {
        return EEPROM::State::COMMUNICATION_ERROR;
    }
    return EEPROM::State::SUCCESS;
}


bool EDIDManager:: setSlotWriteProtect(uint8_t slot, bool enable) {
    // Check if the slot is valid (Slot 0 to 14, 15 has the meta data)
    if (slot >= EDID_SLOT_COUNT) return false;
    // Read the first byte of the meta data from the eeeprom
    // u8 flags = eeprom.readOneByte(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE);
    u8 flags;
    I2C::State status = eeprom.readBytes(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE, &flags, 1);
    if (status != I2C::State::SUCCESS) {
        return false;
    }
    // Set the write protect bit
    if (enable) {
        flags |= WRITE_PROTECT_MASK;
    } else {
        flags &= ~WRITE_PROTECT_MASK;
    }
    // Write the flags back to the EEPROM
    status = eeprom.writeBytes(EDID_META_DATA_ADDR + slot * EDID_META_DATA_SIZE, &flags, 1);
    if (status != I2C::State::SUCCESS) {
        return false;
    }
    return true;
}


EEPROM::State EDIDManager::readMonitorEDID() {
    // Change the EEPROM Adress by the GPIO, but keep the I2C
    GPIO_SetBits(this->addrCtlPort, this->addrCtlPin);
    eeprom.setI2CAddress(MONITOR_ADDRESS);
    // Cleat the RAM Slot
    this->cleanRamSlot();
    // Read the EDID from the Monitor
    I2C::State status = eeprom.readBytes(0, ramSlot.data, NORMAL_EDID_SIZE);
    // Check if edid is a extended edid
    bool isExtended = false;
    if (ramSlot.edid.extensionFlag > 0){
        // EDID is extended. Read the rest of the EDID
        status = eeprom.readBytes(NORMAL_EDID_SIZE, ramSlot.data + NORMAL_EDID_SIZE, EDID_EXTENSION_SIZE);
        isExtended = true;
    }

    if (status != I2C::State::SUCCESS) {
        //  Abort, set the default address and return
        this->setDefaultAddress();
        return EEPROM::State::MONITOR_NO_ANSWER;
    }
    // Create the Meta Data
    this->cleanRamMetaData();

    // Set the EDID Valid Flag
    ramMetaData.edid_valid = 1;
    ramMetaData.write_protect = 0;
    ramMetaData.clone_status = 1;   
    if (isExtended) {
        ramMetaData.edid_extended = 1;
    } else {
        ramMetaData.edid_extended = 0;
    }


    this->setDefaultAddress();
    return EEPROM::State::SUCCESS;
}


